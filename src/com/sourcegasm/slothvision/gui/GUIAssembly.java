package com.sourcegasm.slothvision.gui;

import com.sourcegasm.slothvision.gstreamer.GStreamerUDPSRCGrabber;
import com.sourcegasm.slothvision.opengl.OpenGLWindow;

import java.awt.image.BufferedImage;

public class GUIAssembly {

	private OpenGLWindow window = null;
	private GStreamerUDPSRCGrabber src1 = null;
	private GStreamerUDPSRCGrabber src2 = null;
	public static boolean running = true;

	public GUIAssembly(GStreamerUDPSRCGrabber src1, GStreamerUDPSRCGrabber src2){
		this.src1 = src1;
		this.src2 = src2;

		window = new OpenGLWindow();

		Thread uiUpdateThread = new Thread(this::updateAssembly);
		uiUpdateThread.setDaemon(true);
		uiUpdateThread.start();
	}

	private void updateAssembly(){

		while(running){
			BufferedImage src_vid1;
			BufferedImage src_vid2;

			src_vid1 = src1.getCurrentFrame();
			src_vid2 = src2.getCurrentFrame();

			if(src_vid1==null){
				src_vid1 = src_vid2;
			}

			if(src_vid2==null){
				src_vid2 = src1.getCurrentFrame();
				src_vid1 = src_vid2;
			}

			if(src_vid1!=null) {
				window.leftImage = src_vid1;
				window.rightImage = src_vid2;
			}

			try {
				Thread.sleep(30);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}

		}

	}

	public void showAssembly(){
		window.start();
	}

}
