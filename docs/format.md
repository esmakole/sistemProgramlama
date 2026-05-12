# tarsau Arşiv Formatı (.sau)

## Genel Bakış

`.sau` dosya formatı, birden fazla ASCII metin dosyasını tek bir dosyada saklamak için tasarlanmış basit bir arşiv formatıdır. Sıkıştırma yapılmaz.

## Format Yapısı

```
┌─────────────────────────────────────┐
│           HEADER (Başlık)           │
│  - Magic: "SAU1" (4 byte)          │
│  - Version: int (4 byte)           │
│  - File Count: int (4 byte)        │
├─────────────────────────────────────┤
│        METADATA Bölümü              │
│  Her dosya için:                    │
│  - Filename (256 byte)              │
│  - Size: off_t (8 byte)            │
│  - Permissions: mode_t (4 byte)    │
├─────────────────────────────────────┤
│        CONTENT (İçerik) Bölümü      │
│  Her dosya için:                    │
│  - Ham dosya verisi (size byte)     │
└─────────────────────────────────────┘
```

## Notlar

- Yalnızca ASCII metin dosyaları desteklenir
- Maksimum 32 dosya arşivlenebilir
- Dosya izinleri (Unix mode) korunur
