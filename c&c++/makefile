SUBDIR = lame.extractor lame.scanner lame.mt.scanner

all clean x86 x64 b32 b64 l32 l64:
	@for subdir in $(SUBDIR);\
	do \
		echo "making $@ in $$subdir";\
		(cd $$subdir && make $@) || exit 1;\
	done

