package engine;

public class LameInfo {
	public String engine_version;
	public String virus_db_version;
	
	public LameInfo(){
	}
	
	public LameInfo(LameInfo li) {
		if(null == li) return;
		engine_version = li.engine_version;
		virus_db_version = li.virus_db_version;
	}
}
