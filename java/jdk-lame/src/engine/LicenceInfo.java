package engine;

public class LicenceInfo {
	public String Version;
	public String Owner;
	public String Date;
	public String Authm;
	public String Data;
	
	public LicenceInfo(){
		
	}
	
	public LicenceInfo(LicenceInfo li) {
		if(null == li) return;
		Version = li.Version;
		Owner = li.Owner;
		Date = li.Date;
		Authm = li.Authm;
		Data = li.Data;
		}
}
