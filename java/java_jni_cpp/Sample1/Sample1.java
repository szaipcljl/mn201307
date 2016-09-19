//有4个native方法, 分别是4种类型的参数, int, boolean, String, int[].

public class Sample1 {
	public native int intMethod(int n);
	public native boolean booleanMethod(boolean bool);
	public native String stringMethod(String text);
	public native int intArrayMethod(int[] intArray);

	public static void main(String[] args) {
		System.loadLibrary("Sample1"); //加载动态类库

		Sample1 sample = new Sample1();

		int square = sample.intMethod(5);
		boolean bool = sample.booleanMethod(true);
		String text = sample.stringMethod("Java");
		int sum = sample.intArrayMethod(new int[]{1,2,3,4,5,8,13});

		System.out.println("intMethod: " + square);
		System.out.println("booleanMethod: " + bool);
		System.out.println("stringMethod: " + text);
		System.out.println("intArrayMethod: " + sum);
	}
}
