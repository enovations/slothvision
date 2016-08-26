package com.sourcegasm.slothvision.control;

import com.sourcegasm.slothvision.Launcher;
import com.sourcegasm.slothvision.control.modes.ModeSwitcher;
import com.sourcegasm.slothvision.oculus.Euler;

import java.nio.ByteBuffer;

public class MainController implements Runnable {

	private Thread thread = new Thread(this);
	private ROSControl control = null;

	public static boolean running = true;

	private LowPassFilter panFilter = new LowPassFilter(2);
	private LowPassFilter tiltFilter = new LowPassFilter(2);

	private PID yaw = new PID(0.0016f, 0, 0, 0, PID.Direction.NORMAL);
	private PID speed = new PID(0.95f, 0.0001f, 0, 0.1f, PID.Direction.REVERSED);

    private PID panPid = new PID(0.0016f, 0, 0, 0, PID.Direction.NORMAL);
    private PID tiltPid = new PID(0.0016f, 0, 0, 0, PID.Direction.NORMAL);

	private ModeSwitcher switcher = new ModeSwitcher();

	private int oldState1 = 0;

	public MainController() {

		control = new ROSControl();

	}

	@Override public void run() {

		while (running) {

			long milis = System.currentTimeMillis();

			ByteBuffer buttons = Launcher.joystick.buttonBuffer;

			if (buttons != null) {

				int curr1 = buttons.get(1);
				if (oldState1 != curr1) {
					if (curr1 == 1) {
						switcher.nextMode();
					}
				}

				oldState1 = curr1;

				//while (buttons.hasRemaining())
				//	System.out.print(buttons.get());
				//System.out.println();

			}

			if (Launcher.piROSConnector.data.mode == 0) {

				Euler euler = Launcher.hmdSensors.getEulerAngles();
				if (euler != null) {
					control.pan = panFilter.calculate(euler.yaw * 80.0);
					control.tilt = Limiter.limitValues(tiltFilter.calculate(euler.pitch * -90.0), 3500);
				}

				if (Launcher.joystick.button_drive == 1) {
					control.speed = Launcher.joystick.left_ud;
					control.steer = Launcher.joystick.left_lr;
				} else {
					control.speed = control.steer = 0;
				}

			} else if (Launcher.piROSConnector.data.mode == 1) {

				control.pan = 0;
				control.tilt = 0;

				if (Launcher.joystick.button_drive == 1) {
					control.speed = Launcher.joystick.left_ud;
					control.steer = Launcher.joystick.left_lr;
				} else {
					control.speed = control.steer = 0;
				}

			} else if (Launcher.piROSConnector.data.mode == 2) {

				control.pan = 0;
				control.tilt = 0;

				if (Launcher.piROSConnector.data.marker_x != 0 && Launcher.piROSConnector.data.marker_distance != 0) {
					control.steer = yaw.calculate(Launcher.piROSConnector.data.marker_x, 450);
					double val = (1.0 / Launcher.piROSConnector.data.marker_distance) * 100.0;
					double speedVal = 0;
					if (!(val < 0.5 || val > 1.4)) {
						speedVal = speed.calculate((float) val, 0.9f);
					}
					System.out.println(speedVal);
					control.speed = Limiter.limitValues(speedVal, 0.8);
				} else {
					if (Launcher.joystick.button_drive == 1) {
						control.speed = Launcher.joystick.left_ud;
						control.steer = Launcher.joystick.left_lr;
					} else {
						control.speed = control.steer = 0;
					}
				}
			} else if (Launcher.piROSConnector.data.mode == 3) {

				if (Launcher.piROSConnector.data.marker_x != 0 && Launcher.piROSConnector.data.marker_distance != 0) {


				    control.pan = panPid.calculate(Launcher.piROSConnector.data.marker_x, 450);
					System.out.println(control.pan);

				} else {

					if (Launcher.joystick.button_drive == 1) {
						control.speed = Launcher.joystick.left_ud;
						control.steer = Launcher.joystick.left_lr;
					} else {
						control.speed = control.steer = 0;
					}

					Euler euler = Launcher.hmdSensors.getEulerAngles();
					if (euler != null) {
						control.pan = panFilter.calculate(euler.yaw * 80.0);
						control.tilt = Limiter.limitValues(tiltFilter.calculate(euler.pitch * -90.0), 3500);
					}
				}
			}

			//send data updates
			control.sendUpdate();

			try {
				milis = System.currentTimeMillis() - milis;
				int sleep = (int) (30 - milis);
				if (sleep > 0)
					Thread.sleep(sleep);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

	}

	public void start() {
		thread.start();
	}
}
