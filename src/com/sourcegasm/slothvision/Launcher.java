package com.sourcegasm.slothvision;

import com.sourcegasm.slothvision.control.JoystickData;
import com.sourcegasm.slothvision.control.MainController;
import com.sourcegasm.slothvision.control.SlothEyesController;
import com.sourcegasm.slothvision.gstreamer.GStreamerUDPSRCGrabber;
import com.sourcegasm.slothvision.gui.GUIAssembly;
import com.sourcegasm.slothvision.oculus.HMDSensors;
import com.sourcegasm.slothvision.pimanagement.PiCamera;
import com.sourcegasm.slothvision.pimanagement.PiROSConnector;

import javax.swing.*;
import java.io.IOException;
import java.lang.reflect.Field;
import java.net.InetAddress;

public class Launcher {

    public static final HMDSensors hmdSensors = new HMDSensors(40);
    public static final JoystickData joystick = new JoystickData();

    public static PiROSConnector piROSConnector = null;

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

        piROSConnector = new PiROSConnector("wlp2s0");
        piROSConnector.runConnector();

        GStreamerUDPSRCGrabber.initGST(args);
        GStreamerUDPSRCGrabber src1 = new GStreamerUDPSRCGrabber(5000);
        GStreamerUDPSRCGrabber src2 = new GStreamerUDPSRCGrabber(5001);

        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            hmdSensors.kill();
            src1.killPipe();
            src2.killPipe();
            camera.kill();
            piROSConnector.kill();
            GUIAssembly.running = false;
            MainController.running = false;
        }));

        MainController controller = new MainController();
        controller.start();

        if (!isSlothAccessible()) {
            JOptionPane.showMessageDialog(null,
                    "<html><h2>Sloth went full John Cena!</h2>Sloth controller was not able to talk to sloth.",
                    "Sloth error", JOptionPane.ERROR_MESSAGE);
        }

        GUIAssembly assembly = new GUIAssembly(src1, src2);

        SlothEyesController slothEyesController = new SlothEyesController(assembly);
        slothEyesController.start(20);

        assembly.showAssembly();


    }

    private static boolean isSlothAccessible() {
        try {
            return InetAddress.getByName("192.168.0.100").isReachable(500);
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
    }

}
