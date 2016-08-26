package com.sourcegasm.slothvision.control;

import com.sourcegasm.slothvision.Launcher;
import com.sourcegasm.slothvision.gui.GUIAssembly;
import com.sourcegasm.slothvision.opensv.LocationData;
import com.sourcegasm.slothvision.opensv.SlothEyes;

import java.awt.image.BufferedImage;

public class SlothEyesController {
	private GUIAssembly guiAssembly;
	private SlothEyes slothEyes1 = new SlothEyes();
	private SlothEyes slothEyes2 = new SlothEyes();

	public SlothEyesController(GUIAssembly guiAssembly) {
		this.guiAssembly = guiAssembly;
	}

	public void start(int frequency) {
		int sleep = 1000 / frequency;
		new Thread(() -> {
			while (true) {
				calculateDistance();
				try {
					Thread.sleep(sleep);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}).start();
	}

	public double calculateDistance() {
		BufferedImage image1 = guiAssembly.src1.getCurrentFrame();
		BufferedImage image2 = guiAssembly.src2.getCurrentFrame();

		if (image1 == null || image2 == null) {
			//System.out.println("No image received!");
			return 0;
		}

		LocationData data1 = slothEyes1.getMarkerData(image1, false);
		LocationData data2 = slothEyes2.getMarkerData(image2, false);

		double distance = Math.abs(data1.x - data2.x);

		Launcher.piROSConnector.data.markerLoc = data1;

		Launcher.piROSConnector.data.marker_distance = (int) distance;
		Launcher.piROSConnector.data.marker_x = (int) (((double) data1.x + (double) data2.x) / 2.0);

		return 0;
	}

}