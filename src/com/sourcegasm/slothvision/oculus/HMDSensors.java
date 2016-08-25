package com.sourcegasm.slothvision.oculus;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class HMDSensors {

	private boolean isReady = false;
	private Euler currentEuler = null;

	private Process process = null;

	public HMDSensors(int frequency) {
		try {
			//process = Runtime.getRuntime().exec("echo temp");
			process = Runtime.getRuntime().exec("python2 -u ./oculus_sensors.py " + frequency);
			final BufferedReader stdError = new BufferedReader(new InputStreamReader(process.getErrorStream()));
			final BufferedReader stdInput = new BufferedReader(new InputStreamReader(process.getInputStream()));

			//handle error stream
			new Thread(() -> {
				String s = null;
				try {
					while ((s = stdError.readLine()) != null) {
						System.out.println("proc error: " + s);
					}
				} catch (IOException e) {
					System.out.println("HMDSensors just died!");
				}
			}).start();

			//handle python output
			new Thread(() -> {
				String s = null;
				try {
					while ((s = stdInput.readLine()) != null) {
						if (!isReady && s.contains("frequency")) {
							isReady = true;
						} else if (isReady) {
							try {
								currentEuler = new Quaternion(s).toEuler();
							} catch (Exception e) {
								e.printStackTrace();
							}
						}
					}
				} catch (IOException e) {
					e.printStackTrace();
				} finally {
					try {
						stdError.close();
						stdInput.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			}).start();

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void kill() {
		try {
			process.destroy();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public boolean isReady() {
		return isReady;
	}

	public Euler getEulerAngles() {
		return currentEuler;
	}
}
