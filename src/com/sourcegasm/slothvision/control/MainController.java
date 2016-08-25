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

	ModeSwitcher switcher = new ModeSwitcher();

	int oldState1 = 0;

	public MainController() {

		control = new ROSControl();

	}

	@Override public void run() {

		while (running) {

			long milis = System.currentTimeMillis();

			ByteBuffer buttons = Launcher.joystick.buttonBuffer;

			if(buttons!=null) {

				//while (buttons.hasRemaining())
				//	System.out.print(buttons.get());
				//System.out.println();

			}

			Euler euler = Launcher.hmdSensors.getEulerAngles();
			if (euler != null) {
				control.pan = panFilter.calculate(euler.yaw * 80.0);
				control.tilt = Limiter.limitValues(tiltFilter.calculate(euler.pitch * -90.0), 3500);
			}

			if(Launcher.piROSConnector.data.mode == 0) {

				if (Launcher.joystick.button_drive == 1) {
					control.speed = Launcher.joystick.left_ud;
					control.steer = Launcher.joystick.left_lr;
				} else {
					control.speed = control.steer = 0;
				}

			}else if(Launcher.piROSConnector.data.mode == 1) {
				//cv controls auto
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
