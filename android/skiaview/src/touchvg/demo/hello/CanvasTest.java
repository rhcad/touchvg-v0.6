package touchvg.demo.hello;

import touchvg.skiaview.GiCanvasBase;
import touchvg.skiaview.Floats;
import android.util.Log;

public class CanvasTest extends GiCanvasBase {
	public boolean rawLine(float x1, float y1, float x2, float y2) {
		Log.d("canvas", "rawLine(" + x1 + ", " + y1 + ", " + x2 + ", " + y2);
		return true;
	}
	public boolean rawLines(Floats pxs) {
		Log.d("canvas", "rawLines:" + pxs.count());
		for (int i = pxs.count() - 1; i >= 0; i--)
			Log.d("canvas", "rawLines " + i + ": " + pxs.get(i));
		return true;
	}
}
