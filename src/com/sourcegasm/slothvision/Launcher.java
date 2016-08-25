package com.sourcegasm.slothvision;

import com.sourcegasm.slothvision.control.JoystickData;
import com.sourcegasm.slothvision.control.MainController;
import com.sourcegasm.slothvision.gstreamer.GStreamerUDPSRCGrabber;
import com.sourcegasm.slothvision.gui.GUIAssembly;
import com.sourcegasm.slothvision.oculus.HMDSensors;
import com.sourcegasm.slothvision.picamera.PiCamera;

import java.lang.reflect.Field;

public class Launcher {

	public static final HMDSensors hmdSensors = new HMDSensors(40);
	public static final JoystickData joystick = new JoystickData();

	public static void main(String[] args) {

		try {
			System.setProperty("java.library.path", "./lib/native");
			Field sysPathsField = ClassLoader.class.getDeclaredField("sys_paths");
			sysPathsField.setAccessible(true);
			sysPathsField.set(null, null);
		} catch (Exception e) {
			e.printStackTrace();
		}

		PiCamera camera = new PiCamera("wlp2s0");
		camera.run();

		GStreamerUDPSRCGrabber.initGST(args);
		GStreamerUDPSRCGrabber src1 = new GStreamerUDPSRCGrabber(5000);
		GStreamerUDPSRCGrabber src2 = new GStreamerUDPSRCGrabber(5001);

		Runtime.getRuntime().addShutdownHook(new Thread(()->{
			hmdSensors.kill();
			src1.killPipe();
			src2.killPipe();
			camera.kill();
			GUIAssembly.running = false;
			MainController.running = false;
		}));

		MainController controller = new MainController();
		controller.start();

		GUIAssembly assembly = new GUIAssembly(src1,src2);
		assembly.showAssembly();

	}

}
