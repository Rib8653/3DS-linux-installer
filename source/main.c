// Linux for 3DS - Installer
// Copies the Linux for 3DS files bundled in RomFS onto the SD card.

#include <3ds.h>
#include <stdio.h>
#include <sys/stat.h>

#define COPY_BUF_SIZE 0x10000

typedef struct {
	const char *src;
	const char *dst;
} InstallFile;

static const InstallFile files[] = {
	{ "romfs:/zImage",                 "sdmc:/linux/zImage" },
	{ "romfs:/nintendo3ds_ctr.dtb",    "sdmc:/linux/nintendo3ds_ctr.dtb" },
	{ "romfs:/nintendo3ds_ktr.dtb",    "sdmc:/linux/nintendo3ds_ktr.dtb" },
	{ "romfs:/arm9linuxfw.bin",        "sdmc:/linux/arm9linuxfw.bin" },
	{ "romfs:/initramfs.cpio.gz",      "sdmc:/linux/initramfs.cpio.gz" },
	{ "romfs:/firm_linux_loader.firm", "sdmc:/luma/payloads/firm_linux_loader.firm" },
};

#define NUM_FILES (sizeof(files) / sizeof(files[0]))

static u8 buf[COPY_BUF_SIZE];

static bool copyFile(const char *src, const char *dst)
{
	FILE *in = fopen(src, "rb");
	if (!in) {
		printf("  MISSING: %s\n", src);
		return false;
	}

	FILE *out = fopen(dst, "wb");
	if (!out) {
		printf("  FAILED to open dest: %s\n", dst);
		fclose(in);
		return false;
	}

	bool ok = true;
	size_t n;
	while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
		if (fwrite(buf, 1, n, out) != n) {
			printf("  WRITE ERROR: %s\n", dst);
			ok = false;
			break;
		}
	}

	fclose(out);
	fclose(in);

	if (ok)
		printf("  OK: %s\n", dst);
	return ok;
}

static void waitForStart(void)
{
	while (aptMainLoop()) {
		hidScanInput();
		if (hidKeysDown() & KEY_START)
			break;
		gspWaitForVBlank();
	}
}

int main(void)
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	romfsInit();

	printf("Linux for 3DS - Installer\n");
	printf("==========================\n\n");
	printf("All files are bundled in this app.\n");
	printf("Press A to install, START to quit.\n");

	bool install = false;
	while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;
		if (kDown & KEY_A) {
			install = true;
			break;
		}
		gspWaitForVBlank();
	}

	if (install) {
		printf("\nCreating folders...\n");
		mkdir("sdmc:/linux", 0777);
		mkdir("sdmc:/luma", 0777);
		mkdir("sdmc:/luma/payloads", 0777);

		printf("\nCopying files...\n");
		int failed = 0;
		for (size_t i = 0; i < NUM_FILES; i++) {
			if (!copyFile(files[i].src, files[i].dst))
				failed++;
		}

		if (failed == 0) {
			printf("\nSUCCESS! All files installed.\n");
			printf("Reboot and pick firm_linux_loader.firm\n");
			printf("from the Luma3DS chainloader menu.\n");
		} else {
			printf("\n%d file(s) failed - see above.\n", failed);
			printf("Make sure sdmc:/linux-installer/ has\n");
			printf("all 6 source files in it.\n");
		}

		printf("\nPress START to quit.\n");
		waitForStart();
	}

	romfsExit();
	gfxExit();
	return 0;
}
