```
cd /mnt/blikvm/package
pyinstaller --onefile --hidden-import=edidconf --collect-submodules=edidconf --paths=/mnt/blikvm/package edidconf/__main__.py
```