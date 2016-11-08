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

		int arr[] = new int[9];
		arr = new int[]{3,8,9,2,9,8,13,1,6};

		System.out.print("the original arr:");
		for (int i = 0; i < arr.length; i++)
			System.out.print(arr[i] + ",");
		System.out.println();

		int sum = sample.intArrayMethod(arr);

		System.out.println("intMethod: " + square);
		System.out.println("booleanMethod: " + bool);
		System.out.println("stringMethod: " + text);
		System.out.println("intArrayMethod: " + sum);

		System.out.print("the new arr:");
		for (int i = 0; i < arr.length; i++)
			System.out.print(arr[i] + ",");
		System.out.println();
	}
}
