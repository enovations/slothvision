package com.sourcegasm.slothvision;

import com.sourcegasm.slothvision.opengl.OpenGLWindow;

import java.lang.reflect.Field;

public class Launcher {

	public static void main(String[] args) {

		try {
			System.setProperty("java.library.path", "./lib/native");

			Field sysPathsField = ClassLoader.class.getDeclaredField("sys_paths");
			sysPathsField.setAccessible(true);
			sysPathsField.set(null, null);
		}catch (Exception e){
			e.printStackTrace();
		}

		OpenGLWindow window = new OpenGLWindow();
		window.start();
	}

}
