package touchvg.demo.hello;

import touchvg.skiaview.GiSkiaView;
import touchvg.skiaview.GiCanvasBase;
import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class testy extends Activity {
	static {
		System.loadLibrary("skiaview");
	}
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        TextView t = (TextView) findViewById(R.id.id);
        
        GiCanvasBase c = new GiCanvasBase();
        GiSkiaView v = new GiSkiaView(c);
        v.onSize(100, 200);
        t.setText("Hello TouchVG! w=" + v.getWidth() + " h=" + v.getHeight());
        v.delete();
        c.delete();
    }
}
