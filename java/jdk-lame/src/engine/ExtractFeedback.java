package engine;

public interface ExtractFeedback {
	public abstract long ExtEnterFile(String file_name, String format, long depth, long FCLS, long handle, Object usrdata);
	public abstract void ExtLeaveFile(String file_name, String format, long depth, long FCLS, long handle, Object usrdata, long l);
}
