package com.sourcegasm.slothvision.control;

import com.sourcegasm.slothvision.Launcher;
import com.sourcegasm.slothvision.oculus.Euler;

public class MainController implements Runnable {

	private Thread thread = new Thread(this);
	private ROSControl control = null;

	public static boolean running = true;

	private LowPassFilter panFilter = new LowPassFilter(2);
	private LowPassFilter tiltFilter = new LowPassFilter(2);

	public MainController() {

		control = new ROSControl();

	}

	@Override public void run() {

		while (running) {

			long milis = System.currentTimeMillis();

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
