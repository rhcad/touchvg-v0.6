package touchvg.demo;

import touchvg.view.PaintView;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;

public class testy extends Activity {
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        final testy mHandler = this;
        
        View buttonAdd = this.findViewById(R.id.addshapes_button);
        buttonAdd.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
        		mHandler.onAddShapes();
        	}
    	});
    	
    	View buttonClear = this.findViewById(R.id.clearshapes_button);
    	buttonClear.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
        		mHandler.onClearShapes();
        	}
    	});
    }
    
    private void onAddShapes() {
    	PaintView view = (PaintView)this.findViewById(R.id.paintView);
    	view.getCoreView().addTestingShapes();
    	view.getCoreView().setCommandName("select");
    	view.invalidate();
    }
    
    private void onClearShapes() {
    	PaintView view = (PaintView)this.findViewById(R.id.paintView);
    	view.getCoreView().loadShapes(null);
    	view.getCoreView().setCommandName("splines");
    	view.invalidate();
    }
}
