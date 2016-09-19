public class TestJNI {
	static{
		System.loadLibrary("diaoyong");  //程序在加载时，自动加载libdiaoyong.so库
	} 
	public native void set(int value);      //声明原生函数。注意要添加native关键字
	public native int get();
	public static void main(String[] args) { 
		TestJNI test=new TestJNI(); 
		test.set(1); 
		System.out.println(test.get()); 
	}
}   
