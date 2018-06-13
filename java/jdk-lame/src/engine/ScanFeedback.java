package engine;

public interface ScanFeedback {
	public abstract long DtEnterFile(String file_name, long depth, Object usr_data);
	public abstract void DtLeaveFile(String file_name, long depth, Object usr_data, long l);
	public abstract long DtAlarm(String file_name, ScanResult sr, Object usr_data);
}
