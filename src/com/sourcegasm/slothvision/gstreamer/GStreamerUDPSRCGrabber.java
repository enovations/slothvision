package com.sourcegasm.slothvision.gstreamer;

import org.gstreamer.*;
import org.gstreamer.elements.BaseSrc;
import org.gstreamer.swing.VideoComponent;

import javax.swing.*;
import java.awt.image.BufferedImage;

public class GStreamerUDPSRCGrabber {

	private VideoComponent panel = new VideoComponent();
	private Pipeline pipeline = new Pipeline("pipeline");

	public GStreamerUDPSRCGrabber(int port) {
		final BaseSrc fakesrc = (BaseSrc) ElementFactory.make("udpsrc", "source");
		fakesrc.set("port", port);
		fakesrc.setCaps(Caps.fromString(
				"application/x-rtp, media=(string)video, clock-rate=(int)90000, payload=(int)96, encoding-name=(string)H264"));

		final Element depay = ElementFactory.make("rtph264depay", "depay");
		final Element decode = ElementFactory.make("ffdec_h264", "decode");
		final Element colorspace = ElementFactory.make("ffmpegcolorspace", "colorspace");

		SwingUtilities.invokeLater(() -> {
			JFrame frame = new JFrame();
			frame.add(panel);
			Element videosink = panel.getElement();

			pipeline.addMany(fakesrc, depay, decode, colorspace, videosink);
			Element.linkMany(fakesrc, depay, decode, colorspace, videosink);

			frame.setSize(30, 30);
			frame.setVisible(true);
			pipeline.play();
		});

	}

	public BufferedImage getCurrentFrame(){
		return panel.currentImage;
	}

	public static void initGST(String[] args) {
		Gst.init("GStreamerUDPSRCGrabber", args);
	}

	public void killPipe() {
		pipeline.setState(State.NULL);
	}

}
