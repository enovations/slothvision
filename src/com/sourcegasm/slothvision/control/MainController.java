package com.sourcegasm.slothvision.control;

import com.sourcegasm.slothvision.Launcher;
import com.sourcegasm.slothvision.oculus.Euler;
import org.lwjgl.openal.LOKIAudioChannel;

public class MainController implements Runnable {

	private Thread thread = new Thread(this);
	ROSControl control = null;

	public static boolean running = true;

	LowPassFilter panFilter = new LowPassFilter(2);
	LowPassFilter tiltFilter = new LowPassFilter(2);

	public MainController() {

		control = new ROSControl();

	}

	@Override public void run() {

		while (running) {

			long milis = System.currentTimeMillis();

			Euler euler = Launcher.hmdSensors.getEulerAngles();
			if(euler!=null){
				control.pan = panFilter.calculate(euler.yaw*80.0);
				control.tilt = tiltFilter.calculate(euler.pitch*-90.0);
			}

			control.speed = Launcher.joystick.left_ud;
			control.steer = Launcher.joystick.left_lr;

			//send data updates
			control.sendUpdate();

			try {
				milis = System.currentTimeMillis() - milis;
				Thread.sleep(30-milis);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

	}

	public void start() {
		thread.start();
	}
}
