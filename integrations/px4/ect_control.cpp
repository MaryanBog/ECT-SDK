#include <cstdio>
#include <px4_platform_common/log.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

#include <drivers/drv_hrt.h>

#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/offboard_control_mode.h>
#include <uORB/topics/trajectory_setpoint.h>

#include <ect_sdk.hpp>
#include <cmath>

using namespace ect::sdk;
using namespace time_literals;

class EctControl : public ModuleBase, public ModuleParams, public px4::ScheduledWorkItem
{
public:
    static Descriptor desc;

    EctControl();
    ~EctControl() override = default;

    static int task_spawn(int argc, char *argv[]);
    static int custom_command(int argc, char *argv[]);
    static int print_usage(const char *reason = nullptr);

    bool init();
    int print_status() override;

private:
    void Run() override;

    LinearFOperator _f;
    LinearEOperator _e{0.8};
    LinearFInvOperator _finv;
    LinearGOperator _g{1.5, -2.0, 2.0};
    Controller _ctrl_x{_f, _e, _finv, _g};
    Controller _ctrl_y{_f, _e, _finv, _g};
    Controller _ctrl_z{_f, _e, _finv, _g};

    uORB::Subscription _vehicle_local_position_sub{ORB_ID(vehicle_local_position)};

    uORB::Publication<offboard_control_mode_s> _offboard_control_mode_pub{
        ORB_ID(offboard_control_mode)
    };

    uORB::Publication<trajectory_setpoint_s> _trajectory_setpoint_pub{
        ORB_ID(trajectory_setpoint)
    };

    const float _target_x{0.0f};
    const float _target_y{0.0f};
    const float _target_z{-2.5f};

    int _test_mode{0};  // 0=normal, 1=noise, 2=tracking
};

ModuleBase::Descriptor EctControl::desc{task_spawn, custom_command, print_usage};

EctControl::EctControl() :
    ModuleParams(nullptr),
    ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::test1)
{
}

bool EctControl::init()
{
    ScheduleOnInterval(20_ms);
    return true;
}

void EctControl::Run()
{
    if (should_exit()) {
        ScheduleClear();
        exit_and_cleanup(desc);
        return;
    }

    vehicle_local_position_s pos{};

    if (!_vehicle_local_position_sub.copy(&pos)) {
        return;
    }

    if (!PX4_ISFINITE(pos.x) || !PX4_ISFINITE(pos.y) || !PX4_ISFINITE(pos.z) ||
        !PX4_ISFINITE(pos.vx) || !PX4_ISFINITE(pos.vy) || !PX4_ISFINITE(pos.vz)) {
        return;
    }

    const hrt_abstime now_us = hrt_absolute_time();
    const float now_s = static_cast<float>(now_us) * 1e-6f;

    // SITL-only runtime test control.
    // /tmp/ect_test_mode: 0=normal, 1=noise, 2=tracking.
    static hrt_abstime next_mode_poll_us{0};

    if (now_us >= next_mode_poll_us) {
        next_mode_poll_us = now_us + 100000;

        FILE *fp = fopen("/tmp/ect_test_mode", "r");

        if (fp != nullptr) {
            int requested_mode = 0;

            if (fscanf(fp, "%d", &requested_mode) == 1 &&
                requested_mode >= 0 && requested_mode <= 2) {
                _test_mode = requested_mode;
            }

            fclose(fp);
        }
    }

    float nx = 0.0f;
    float ny = 0.0f;
    float nz = 0.0f;

    if (_test_mode == 1) {
        nx = 0.15f * sinf(7.0f * now_s) + 0.07f * sinf(19.0f * now_s);
        ny = 0.15f * sinf(8.0f * now_s) + 0.07f * sinf(23.0f * now_s);
        nz = 0.10f * sinf(6.0f * now_s) + 0.05f * sinf(17.0f * now_s);
    }

    float target_x = _target_x;
    float target_y = _target_y;
    float target_z = _target_z;

    if (_test_mode == 2) {
        target_x = sinf(0.25f * now_s);
        target_y = cosf(0.25f * now_s);
        target_z = -2.5f + 0.5f * sinf(0.18f * now_s);
    }

    const float ect_x =
        static_cast<float>(_ctrl_x.update(target_x - pos.x + nx));

    const float ect_y =
        static_cast<float>(_ctrl_y.update(target_y - pos.y + ny));

    const float ect_z =
        static_cast<float>(_ctrl_z.update(target_z - pos.z + nz));

    // Velocity damping. This is not an integral term and introduces no
    // accumulated controller state.
    constexpr float kd_xy = 0.8f;
    constexpr float kd_z  = 1.2f;

    const float ax = ect_x - kd_xy * pos.vx;
    const float ay = ect_y - kd_xy * pos.vy;
    const float az = ect_z - kd_z  * pos.vz;

    offboard_control_mode_s mode{};
    mode.timestamp = hrt_absolute_time();
    mode.position = false;
    mode.velocity = false;
    mode.acceleration = true;
    mode.attitude = false;
    mode.body_rate = false;
    mode.thrust_and_torque = false;
    mode.direct_actuator = false;

    _offboard_control_mode_pub.publish(mode);

    trajectory_setpoint_s sp{};
    sp.timestamp = hrt_absolute_time();

    sp.position[0] = NAN;
    sp.position[1] = NAN;
    sp.position[2] = NAN;

    sp.velocity[0] = NAN;
    sp.velocity[1] = NAN;
    sp.velocity[2] = NAN;

    sp.acceleration[0] = ax;
    sp.acceleration[1] = ay;
    sp.acceleration[2] = az;

    sp.jerk[0] = NAN;
    sp.jerk[1] = NAN;
    sp.jerk[2] = NAN;

    sp.yaw = NAN;
    sp.yawspeed = NAN;

    _trajectory_setpoint_pub.publish(sp);
}

int EctControl::task_spawn(int argc, char *argv[])
{
    EctControl *instance = new EctControl();

    if (instance) {
        desc.object.store(instance);
        desc.task_id = task_id_is_work_queue;

        if (instance->init()) {
            PX4_INFO("ECT control started: target_z=%.2f m", static_cast<double>(instance->_target_z));
            return PX4_OK;
        }
    } else {
        PX4_ERR("alloc failed");
    }

    delete instance;
    desc.object.store(nullptr);
    desc.task_id = -1;

    return PX4_ERROR;
}

int EctControl::print_status()
{
    PX4_INFO("ECT control running");
    return 0;
}

int EctControl::custom_command(int argc, char *argv[])
{
    return print_usage("unknown command");
}

int EctControl::print_usage(const char *reason)
{
    if (reason) {
        PX4_WARN("%s", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
ECT-SDK control module.
)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("ect_control", "example");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_COMMAND("stop");
    PRINT_MODULE_USAGE_COMMAND("status");

    return 0;
}


extern "C" __EXPORT int ect_control_main(int argc, char *argv[])
{
    return ModuleBase::main(EctControl::desc, argc, argv);
}
