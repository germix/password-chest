# Password Chest

![logo](proj/res/MainImage.png)

An application to store passwords.

## Languages

- English
- Spanish

## Development

### Translations

- Update

```bash
lupdate.exe proj\PasswordChest.pro
```

- Update (No obsolete)

```bash
lupdate.exe proj\PasswordChest.pro -no-obsolete
```

- Release

```bash
lrelease.exe proj\PasswordChest.pro
MOVE /Y proj\translate\*.qm bin\translations\
COPY /Y proj\translate\*.png bin\translations\
```
