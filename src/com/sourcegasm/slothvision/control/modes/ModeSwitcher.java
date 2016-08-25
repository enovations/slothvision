package com.sourcegasm.slothvision.control.modes;

import com.sourcegasm.slothvision.Launcher;

public class ModeSwitcher {

	int mode = 0;
	int maxMode = 1;

	public void nextMode() {

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
			return "manual";
		case 1:
			return "follow";
		default:
			return "unknown";
		}

	}

}
