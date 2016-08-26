package com.sourcegasm.slothvision.control.modes;

import com.sourcegasm.slothvision.Launcher;

public class ModeSwitcher {

	private int mode = 0;

	public void nextMode() {

		int maxMode = 2;
		if (mode == maxMode) {
			mode = 0;
		} else {
			mode++;
		}

		Launcher.piROSConnector.data.mode = mode;

	}

	public static String getModeName(int mode) {

		switch (mode) {
		case 0:
			return "manual acc";
		case 1:
			return "manual fix";
		case 2:
			return "follow pt";
		default:
			return "unknown";
		}

	}

}
