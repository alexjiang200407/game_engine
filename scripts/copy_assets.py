import shutil, filecmp, os, sys

src = sys.argv[1]
dst = sys.argv[2]

for root, dirs, files in os.walk(src):
    rel = os.path.relpath(root, src)
    dst_dir = os.path.join(dst, rel)
    os.makedirs(dst_dir, exist_ok=True)
    for f in files:
        src_file = os.path.join(root, f)
        dst_file = os.path.join(dst_dir, f)
        if not os.path.exists(dst_file) or not filecmp.cmp(src_file, dst_file, shallow=False):
            shutil.copy2(src_file, dst_file)