package com.ardikars.jxnet;

import com.ardikars.jxnet.util.DefaultLibraryLoader;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

@RunWith(JUnit4.class)
public class LoaderTest {

	public static class Initializer implements ApplicationInitializer {

		@Override
		public void initialize(Context context) {
			context.addLibrary(new DefaultLibraryLoader());
		}

	}

	@Test
	public void test01LoadLibrary() {
		Application.run("TestApp", "0.0.1", Initializer.class, new ApplicationContext());
		assert true;
	}

}
