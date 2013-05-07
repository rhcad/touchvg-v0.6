package touchvg.demo;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import touchvg.view.MyPaintView;
import touchvg.view.PaintView;
import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

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
    private Button buttonShape;     //形状按钮
    private Button captureBtn;
    private Button buttonPicture;
    public String mFilePath = "mnt/sdcard/touchVG";
    private static final int REQUEST_CAPTURE = 0x01;
    private static final int REQUEST_LOCAL = 0x02;

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK) {
            switch (requestCode) {
            case REQUEST_CAPTURE:
                String name = saveCapturePicture(data);
                if (name != null && !name.equals(""))
                {
                    this.mView.addImage(name);
                }
                break;
            case REQUEST_LOCAL:
                String name1 = saveLocalPicture(data);
                if (name1 != null && !name1.equals(""))
                {
                    this.mView.addImage(name1);
                }
                break;
            default:
                break;
            }
        }
    }

    private String saveLocalPicture(Intent data) {
        if (data != null) {
            Cursor cursor = getContentResolver().query(data.getData(), null,
                    null, null, null);
            cursor.moveToFirst();
            String photopathString = cursor.getString(cursor
                    .getColumnIndex(MediaStore.Images.Media.DATA));
            String filename = getFileName();
            copyfile(new File(photopathString),new File(filename),true);
            cursor.close();
            String name = new File(filename).getName();
            Log.e("name",""+name);
            return name;
        }
        return null;
    }

    public static void copyfile(File fromFile, File toFile,Boolean rewrite )
    {
        if (!fromFile.exists()) {
            return;
        }
        if (!fromFile.isFile()) {
            return ;
        }
        if (!fromFile.canRead()) {
            return ;
        }
        if (!toFile.getParentFile().exists()) {
            toFile.getParentFile().mkdirs();
        }
        if (toFile.exists() && rewrite) {
            toFile.delete();
        }
        try {
            FileInputStream fosfrom = new FileInputStream(fromFile);
            FileOutputStream fosto = new FileOutputStream(toFile);
            byte bt[] = new byte[1024];
            int c;
            while ((c = fosfrom.read(bt)) > 0) {
                fosto.write(bt, 0, c); //将内容写到新文件当中
            }
            fosfrom.close();
            fosto.close();
        } catch (Exception ex) {

            Log.e("readfile", ex.getMessage());
        }

    }

    private String saveCapturePicture(Intent data) {
        Bitmap photoCache = null;
        Uri uri = data.getData();

        if (uri != null) {
            photoCache = BitmapFactory.decodeFile(uri.getPath());
        }
        if (photoCache == null) {
            Bundle bundle = data.getExtras();
            if (bundle != null) {
                photoCache = (Bitmap) bundle.get("data");
            } else {
                Toast.makeText(this, "拍照失败", Toast.LENGTH_LONG).show();
                return null;
            }
        }
        if (photoCache != null) {
            // 存储到本地
            String fiename = getFileName();
            saveMyBitmap(fiename, photoCache);
            String name = new File(fiename).getName();
            if (!photoCache.isRecycled()) {
                photoCache.recycle();
                photoCache = null;
            }
            return name;
        }
        return null;

    }

    private String getFileName() {
        int count = 0;
        String filename = ((PaintView)this.mView.getView()).getFilePath()
                + File.separatorChar;
        while (true) {
            if (new File(filename + "captuerimage" + count + ".png").exists()) {
                count = count + 1;
            } else {
                return filename + "captuerimage" + count + ".png";
            }
        }
    }

    public void saveMyBitmap(String filePath, Bitmap mBitmap) {
        File f = new File(filePath);
        if (!new File(f.getParent()).exists())
        {
            new File(f.getParent()).mkdirs();
        }
        try {
            f.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }
        FileOutputStream fOut = null;
        try {
            fOut = new FileOutputStream(f);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        mBitmap.compress(Bitmap.CompressFormat.PNG, 100, fOut);
        try {
            fOut.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            fOut.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /// Called when the activity is first created.
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        this.initComponent();
        this.bindEvent();

        if (savedInstanceState == null) {
            mView.loadFromFile(mFilePath + "/1.vg");
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        String filename = mFilePath + "/resume.vg";
        if (mView.saveAsFile(filename)) {
            outState.putString("file", filename);
            outState.putString("cmd", mView.getCommandName());
        }
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        String filename = savedInstanceState.getString("file");
        if (mView.loadFromFile(filename)) {
            mView.setCommandName(savedInstanceState.getString("cmd"));
        }
    }

    private OnClickListener onClickListener = new OnClickListener() {

        public void onClick(View v) {
            switch (v.getId())
            {
            case R.id.selectshapes_button:
                mView.onSelect();
                break;
            case R.id.eraser_button:
                mView.onEraser();
                break;
            case R.id.clearshapes_button:
                mView.clearShapes();
                mView.clearAllImageFiles();
                break;
            case R.id.redPen_button:
                mView.setShapeType("");
                mView.setRedPen();
                break;
            case R.id.yellowPen_button:
                mView.setShapeType("");
                mView.setYellowPen();
                break;
            case R.id.bluePen_button:
                mView.setShapeType("");
                mView.setBluePen();
                break;
            case R.id.thickPen_button:
                mView.setPenThick();
                break;
            case R.id.boldPen_button:
                mView.setPenBold();
                break;
            case R.id.stylePen_button:
                mView.switchPenStyle();
                break;
            case R.id.save_button:
                mView.saveAsFile(mFilePath + "/1.vg");
                mView.clearAllUnusedFiles();
                break;
            case R.id.shapeType_button:
                mView.switchShapeType();
                break;
            case R.id.capture:
                cameraCapture();
                break;
            case R.id.localpic:
                localPicture();
                break;
            }
            buttonShape.setText(mView.getCommandName());
        }
    };

    /// 事件监听器方法，为按钮绑定方法
    private void bindEvent()
    {
        buttonSelect.setOnClickListener(onClickListener);
        buttonClear.setOnClickListener(onClickListener);
        buttonRed.setOnClickListener(onClickListener);
        buttonBlue.setOnClickListener(onClickListener);
        buttonYellow.setOnClickListener(onClickListener);
        buttonBoldPen.setOnClickListener(onClickListener);
        buttonThickPen.setOnClickListener(onClickListener);
        buttonEraser.setOnClickListener(onClickListener);
        buttonStyle.setOnClickListener(onClickListener);
        buttonSave.setOnClickListener(onClickListener);
        buttonShape.setOnClickListener(onClickListener);
        captureBtn.setOnClickListener(onClickListener);
        buttonPicture.setOnClickListener(onClickListener);
    }

    private void localPicture()
    {
        Intent intent3 = new Intent(
                Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        intent3.setType("image/*");

        startActivityForResult(intent3, REQUEST_LOCAL);
    }

    private void cameraCapture()
    {
        String state = Environment.getExternalStorageState();
        if (state.equals(Environment.MEDIA_MOUNTED)) {
            Intent intent = new Intent("android.media.action.IMAGE_CAPTURE");
            startActivityForResult(intent, REQUEST_CAPTURE);
        } else {
            Toast.makeText(this, "没有SD卡", Toast.LENGTH_LONG).show();
        }
    }

    private void initComponent()
    {
        mView = new MyPaintView(this.findViewById(R.id.paintView),this);
        mView.setBkColor(Color.GRAY);
        mView.setBitmapIDs(R.drawable.vgdot1, R.drawable.vgdot2, 0);

        buttonSelect = (Button) this.findViewById(R.id.selectshapes_button);
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
        captureBtn  = (Button) this.findViewById(R.id.capture);
        buttonPicture = (Button) this.findViewById(R.id.localpic);
    }
}
