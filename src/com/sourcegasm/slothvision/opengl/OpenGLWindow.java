package com.sourcegasm.slothvision.opengl;

import com.sourcegasm.slothvision.Launcher;
import org.lwjgl.glfw.GLFWKeyCallback;
import org.lwjgl.opengl.GL;

import java.awt.image.BufferedImage;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.util.Arrays;

import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;
import static org.lwjgl.opengl.GL20.*;
import static org.lwjgl.opengl.GL30.glBindVertexArray;
import static org.lwjgl.system.MemoryUtil.NULL;

public class OpenGLWindow {

	public boolean running;

	private long window;

	private final int width = 1280, height = 800;

	public GLFWKeyCallback keyCallback = null;

	public Model leftSide, rightSide;

	public BufferedImage rightImage;
	public BufferedImage leftImage;
	int lid, rid;

	public OpenGLWindow() {
        keyCallback = new GLFWKeyCallback() {
			@Override public void invoke(long arg0, int arg1, int arg2, int arg3, int arg4) {
				if (arg1 == GLFW_KEY_ESCAPE && arg3 == GLFW_PRESS) {
					running = false;
				}
			}
		};
	}

	public void start() {
		running = true;
		init();

		long startTime = System.currentTimeMillis();
		long delta = 0;
		final float interval = 1000f / 30f;

		while (running) {
			final long now = System.currentTimeMillis();
			delta += now - startTime;
			startTime = now;

			if (leftImage != null)
				TextureLoader.loadTexture(leftImage, lid);

			if (rightImage != null)
				TextureLoader.loadTexture(rightImage, rid);

			while (delta >= interval) {
				update();
				delta -= interval;
			}
			render();

			if (glfwWindowShouldClose(window)) {
				running = false;
			}
		}
	}

	public void init() {
		if (!glfwInit()) {
			System.err.println("GLFW initialization failed");
		}

		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, "SlothVision", glfwGetMonitors().get(2), NULL);
		//window = glfwCreateWindow(width, height, "SlothVision", glfwGetPrimaryMonitor(), NULL);
		//window = glfwCreateWindow(width, height, "SlothVision", NULL, NULL);

		if (window == NULL) {
			System.err.println("Could not create our Window!");
		}

		glfwMakeContextCurrent(window);
		GL.createCapabilities();
		glfwSwapInterval(1);
		//glfwShowWindow(window);

		glClearColor(0f, 0f, 0f, 1f);

		glfwSetKeyCallback(window, keyCallback);

		BufferedImage texture = new BufferedImage(900, 720, BufferedImage.TYPE_INT_ARGB);

		leftImage = texture;
		rightImage = texture;

		lid = glGenTextures();
		rid = glGenTextures();

		final int[] indices = MeshMaker.indices();
		final float[] textureCoords = MeshMaker.textureCoords();

		leftSide = new Model(MeshMaker.leftMesh(), indices, textureCoords, texture, "shaders/vertexShaderLeft.txt",
				"shaders/fragmentShader.txt", lid);
		rightSide = new Model(MeshMaker.rightMesh(), indices, textureCoords, texture, "shaders/vertexShaderRight.txt",
				"shaders/fragmentShader.txt", rid);
	}

	public void update() {
		// desno levo desno gor desno desno desno gor

		float maxRobotSpeed = 1.0f;

		glfwPollEvents();

		FloatBuffer buffer = glfwGetJoystickAxes(GLFW_JOYSTICK_1);

		if (buffer != null) {

			float robotLeftRight = buffer.get() * -1 * 0.5f;
			if (robotLeftRight < 0.01 && robotLeftRight > -0.01) {
				robotLeftRight = 0;
			}
			float robotUpDown = buffer.get() * -1 * maxRobotSpeed;
			if (robotUpDown < 0.01 && robotUpDown > -0.01) {
				robotUpDown = 0;
			}

			//joystick
			Launcher.joystick.left_lr = robotLeftRight;
			Launcher.joystick.left_ud = robotUpDown;

		} else {
			Launcher.joystick.left_lr = 0;
			Launcher.joystick.left_ud = 0;
		}

		ByteBuffer buttonBuffer = glfwGetJoystickButtons(GLFW_JOYSTICK_1);

		if (buttonBuffer != null) {

			Launcher.joystick.button_drive = buttonBuffer.get(0);

		} else {

			Launcher.joystick.button_drive = 0;

		}

	}

	public void render() {
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Left side
		glUseProgram(leftSide.programID);

		glBindVertexArray(leftSide.vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindTexture(GL_TEXTURE_2D, lid);
		glDrawElements(GL_TRIANGLES, leftSide.size, GL_UNSIGNED_INT, 0);
		//glBindTexture(GL_TEXTURE_2D, 0);
		//glDeleteTextures(leftSide.textureID);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);

		glUseProgram(0);

		// Right side
		glUseProgram(rightSide.programID);

		glBindVertexArray(rightSide.vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindTexture(GL_TEXTURE_2D, rid);
		glDrawElements(GL_TRIANGLES, rightSide.size, GL_UNSIGNED_INT, 0);
		//glBindTexture(GL_TEXTURE_2D, 0);
		//glDeleteTextures(rightSide.textureID);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);

		glUseProgram(0);
	}
}