package touchvg.demo.hello;

import android.app.Activity;
import android.os.Bundle;

public class testy extends Activity {
	static {
		System.loadLibrary("skiaview");
	}
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
    }
}
