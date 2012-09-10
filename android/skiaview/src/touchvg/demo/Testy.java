package touchvg.demo;

import touchvg.skiaview.GiContext;
import touchvg.skiaview.GiSkiaView;
import touchvg.view.PaintView;
import android.R.color;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class Testy extends Activity {
    private PaintView mView;        //组件类
    private GiSkiaView mCoreView;   //内核组件类
    private Button buttonSelect;    //选择图形按钮
    private Button buttonClear;     //清除图形按钮
    private Button buttonRed;       //红色画笔按钮
    private Button buttonBlue;      //蓝色画笔按钮
    private Button buttonThickPen;  //画笔变细按钮
    private Button buttonBoldPen;   //画笔变粗按钮
    private Button buttonYellow;    //黄色画笔按钮
    private Button buttonEraser;    //橡皮按钮
    private Button buttonStyle;     //线型按钮
    private Button buttonShape;    	//形状按钮
    private String commandName = "splines";
    private final Testy mHandler = this;  //视图类实例
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        this.initComponent();   //初始化按钮实例
        this.bindEvent();       //为按钮绑定方法
    }
    
    //事件监听器方法
    private void bindEvent()
    {
        buttonSelect.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onSelectShapes();
            }
        });
        
        buttonClear.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onClearShapes();
            }
        });
        
        buttonRed.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onPenRed();
            }
        });
        
        buttonBlue.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onPenBlue();
            }
        });
        
        buttonYellow.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onPenYellow();
            }
        });
        
        buttonBoldPen.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onPenBold();
            }
        });
        
        buttonThickPen.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onPenThick();
            }
        });
        
        buttonEraser.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onEraser();
            }
        });
        
        buttonStyle.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onPenStyle();
            }
        });
        
        buttonShape.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHandler.onShapeType();
            }
        });
    }

    private void onPenYellow() {
        mCoreView.setCommandName(commandName);
        GiContext ctx = mCoreView.getCurrentContext(false);
        ctx.setLineColor(255, 255, 0);
        mCoreView.applyContext(ctx, 1, 1);
        ctx.delete();
    }
    
    private void onPenBlue() {
        mCoreView.setCommandName(commandName);
        GiContext ctx = mCoreView.getCurrentContext(false);
        ctx.setLineColor(0, 0, 255);
        mCoreView.applyContext(ctx, 1, 1);
        ctx.delete();
    }
    
    private void onPenRed() {
        mCoreView.setCommandName(commandName);
        GiContext ctx = mCoreView.getCurrentContext(false);
        ctx.setLineColor(255, 0, 0);
        mCoreView.applyContext(ctx, 1, 1);
        ctx.delete();
    }
    
    private void onSelectShapes() {
        mCoreView.setCommandName("select");
    }
    
    private void onClearShapes() {
        mCoreView.loadShapes(null);
        mCoreView.setCommandName(commandName);
    }
    
    private void onPenBold() {
        GiContext ctx = mCoreView.getCurrentContext(true);
        ctx.setLineWidth(ctx.getLineWidth() < 0 ? ctx.getLineWidth() - 1 : -1);
        mCoreView.applyContext(ctx, 4, 1);
        ctx.delete();
    }
    
    private void onPenThick() {
        GiContext ctx = mCoreView.getCurrentContext(true);
        ctx.setLineWidth(ctx.getLineWidth() < 0 ? ctx.getLineWidth() + 1 : -1);
        mCoreView.applyContext(ctx, 4, 1);
        ctx.delete();
    }
    
    private void onPenStyle() {
        GiContext ctx = mCoreView.getCurrentContext(true);
        if (ctx.getLineStyle() == 4)
            ctx.setLineStyle(0);
        else
            ctx.setLineStyle(ctx.getLineStyle() + 1);
        mCoreView.applyContext(ctx, 8, 1);
    }
    
    private void onEraser() {
        mCoreView.setCommandName("erase");
    }
    
    private void onShapeType() {
    	String[] names = { "line", "fixedline", "rect", "square", "ellipse", "circle", 
    			"triangle", "diamond", "polygon", "quadrangle", "parallelogram", 
    			"lines", "splines", null };
    	String cmd = mCoreView.getCommandName();
    	
    	int i = 0;
    	for (; names[i] != null && !cmd.equals(names[i]); i++) ;
    	if (names[i] != null) i++;
    	if (names[i] == null) i = 0;
        
        if (mCoreView.setCommandName(names[i])) {
        	buttonShape.setText(names[i]);
        	commandName = names[i];
        }
    }

    private void initComponent()
    {
        mView = (PaintView) this.findViewById(R.id.paintView);
        mView.setBkColor(color.white);
        
        mCoreView = mView.getCoreView();
        buttonSelect  = (Button) this.findViewById(R.id.selectshapes_button);
        buttonClear = (Button) this.findViewById(R.id.clearshapes_button);
        buttonRed = (Button) this.findViewById(R.id.redPen_button);
        buttonBlue = (Button) this.findViewById(R.id.bluePen_button);
        buttonYellow = (Button) this.findViewById(R.id.yellowPen_button);
        buttonBoldPen = (Button) this.findViewById(R.id.boldPen_button);
        buttonThickPen = (Button) this.findViewById(R.id.thickPen_button);
        buttonEraser = (Button) this.findViewById(R.id.eraser_button);
        buttonStyle = (Button) this.findViewById(R.id.stylePen_button);
        buttonShape = (Button) this.findViewById(R.id.shapeType_button);
    }
}
