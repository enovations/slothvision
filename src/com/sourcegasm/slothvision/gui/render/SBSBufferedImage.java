package com.sourcegasm.slothvision.gui.render;

import java.awt.image.BufferedImage;

public class SBSBufferedImage {

	public BufferedImage rightImage;
	public BufferedImage leftImage;

	public SBSBufferedImage(BufferedImage rightImage, BufferedImage leftImage) {
		this.rightImage = rightImage;
		this.leftImage = leftImage;
	}

	public BufferedImage getRightImage() {
		return rightImage;
	}

	public void setRightImage(BufferedImage rightImage) {
		this.rightImage = rightImage;
	}

	public BufferedImage getLeftImage() {
		return leftImage;
	}

	public void setLeftImage(BufferedImage leftImage) {
		this.leftImage = leftImage;
	}

}