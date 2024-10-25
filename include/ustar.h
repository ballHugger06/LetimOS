
ustarHeader* ustarGetNextHeaderBBSafe(ustarHeader* start) {
	u64 size = ustarGetFileSize(start);
	ustarHeader* ptr = ((u8*)start + ((((size + 511) / 512) + 1) * 512));
	if (ptr >= bootboot.initrd_ptr + bootboot.initrd_size) {
		return 0;
	}
	return ptr;
}

ustarHeader* ustarFindFileByPathBBSafe(char* path) {
	ustarHeader* ptr = bootboot.initrd_ptr;

	while(1) {
		ptr = ustarGetNextHeaderBBSafe(ptr);
		if (ptr == 0) {
			return 0;
		}

		if (strsame(path, ptr->filename)) {
			return ptr;
		}
	}
}
