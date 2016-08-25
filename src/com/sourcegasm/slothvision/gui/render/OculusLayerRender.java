package com.sourcegasm.slothvision.gui.render;

import java.awt.image.BufferedImage;

public class OculusLayerRender {

	public static SBSBufferedImage renderAllLayersSBS(BufferedImage l_source, BufferedImage r_source, BufferedImage osdRender) {

		final int imageW = l_source.getWidth();
		final int imageH = l_source.getHeight();

		final BufferedImage leftEye = new BufferedImage(imageW, imageH, BufferedImage.TYPE_INT_RGB);
		final BufferedImage rightEye = new BufferedImage(imageW, imageH, BufferedImage.TYPE_INT_RGB);

		leftEye.getGraphics().drawImage(l_source, 0, 0, imageW, imageH, null);
		rightEye.getGraphics().drawImage(r_source, 0, 0, imageW, imageH, null);

		leftEye.getGraphics().drawImage(osdRender, 18, 0, imageW, imageH, null);
		rightEye.getGraphics().drawImage(osdRender, -18, 0, imageW, imageH, null);

		return new SBSBufferedImage(rightEye, leftEye);
	}

}