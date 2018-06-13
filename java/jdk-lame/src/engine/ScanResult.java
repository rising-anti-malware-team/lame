package engine;

public class ScanResult {
	public int mklass;
	public int vid32;
	public int treat;
	public String kclass;
	public String kclass_desc_a;
	public String kclass_desc_w;
	public String engid;
	public String vname;
	public String hitag;
	
	public ScanResult() {
		
	}
	
	public ScanResult(ScanResult dr) {
		if(null == dr) return;
		mklass = dr.mklass;
		vid32 = dr.vid32;
		treat = dr.treat;
		kclass = dr.kclass;
		kclass_desc_a = dr.kclass_desc_a;
		kclass_desc_w = dr.kclass_desc_w;
		engid = dr.engid;
		vname = dr.vname;
		hitag = dr.hitag;
	}
}
