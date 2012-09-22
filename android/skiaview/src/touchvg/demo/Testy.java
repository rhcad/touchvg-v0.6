package touchvg.demo;

import touchvg.view.MyPaintView;
import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class Testy extends Activity {
    private MyPaintView mView;      //组件类
    private Button buttonSelect;    //选择图形按钮
    private Button buttonClear;     //清除图形按钮
    private Button buttonRed;       //红色画笔按钮
    private Button buttonBlue;      //蓝色画笔按钮
    private Button buttonThickPen;  //画笔变细按钮
    private Button buttonBoldPen;   //画笔变粗按钮
    private Button buttonYellow;    //黄色画笔按钮
    private Button buttonEraser;    //橡皮按钮
    private Button buttonStyle;     //线型按钮
    private Button buttonSave;      //保存按钮
    private Button buttonShape;    	//形状按钮
    
    /// Called when the activity is first created.
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        this.initComponent();
        this.bindEvent();
    }
    
    /// 事件监听器方法，为按钮绑定方法
    private void bindEvent()
    {
        buttonSelect.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mView.onSelect();
            }
        });
        
        buttonClear.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mView.clearShapes();
            }
        });
        
        buttonRed.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mView.setRedPen();
            }
        });
        
        buttonBlue.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mView.setBluePen();
            }
        });
        
        buttonYellow.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mView.setYellowPen();
            }
        });
        
        buttonBoldPen.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mView.setPenBold();
            }
        });
        
        buttonThickPen.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mView.setPenThick();
            }
        });
        
        buttonEraser.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mView.onEraser();
            }
        });
        
        buttonStyle.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mView.switchPenStyle();
            }
        });
        
        buttonSave.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
			}
		});
        
        buttonShape.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	buttonShape.setText(mView.switchShapeType());
            }
        });
    }

    private void initComponent()
    {
    	mView = new MyPaintView(this.findViewById(R.id.paintView));
        mView.setBkColor(Color.GRAY);
        
        buttonSelect  = (Button) this.findViewById(R.id.selectshapes_button);
        buttonClear = (Button) this.findViewById(R.id.clearshapes_button);
        buttonRed = (Button) this.findViewById(R.id.redPen_button);
        buttonBlue = (Button) this.findViewById(R.id.bluePen_button);
        buttonYellow = (Button) this.findViewById(R.id.yellowPen_button);
        buttonBoldPen = (Button) this.findViewById(R.id.boldPen_button);
        buttonThickPen = (Button) this.findViewById(R.id.thickPen_button);
        buttonEraser = (Button) this.findViewById(R.id.eraser_button);
        buttonStyle = (Button) this.findViewById(R.id.stylePen_button);
        buttonSave = (Button) this.findViewById(R.id.save_button);
        buttonShape = (Button) this.findViewById(R.id.shapeType_button);
    }
}
