# tarsau - SAU Arşiv Aracı

Birden fazla ASCII metin dosyasını tek bir `.sau` arşiv dosyasında birleştiren ve geri çıkaran bir arşivleme aracı.

## Proje Yapısı

```
tarsau/
├── src/                    # Kaynak kodlar (.c)
│   ├── main.c              # Ana giriş noktası
│   ├── archive.c           # Arşiv oluşturma modülü
│   ├── extract.c           # Arşiv çıkarma modülü
│   ├── parser.c            # Komut satırı ayrıştırma
│   ├── utils.c             # Yardımcı fonksiyonlar
│   ├── permissions.c       # Dosya izinleri yönetimi
│   └── fileio.c            # Düşük seviye dosya I/O
├── include/                # Başlık dosyaları (.h)
│   ├── tarsau.h            # Ana proje başlığı
│   ├── archive.h
│   ├── extract.h
│   ├── parser.h
│   ├── utils.h
│   ├── permissions.h
│   └── fileio.h
├── obj/                    # Derlenmiş nesne dosyaları
├── bin/                    # Çalıştırılabilir dosya
├── archive/                # Test arşivleri
├── test/                   # Test dosyaları
├── docs/                   # Dokümantasyon
├── Makefile                # GNU Make yapılandırması
└── README.md               # Bu dosya
```

## Derleme

```bash
make            # Derle
make clean      # Temizle
make rebuild    # Temizle ve yeniden derle
make run        # Derle ve çalıştır
make test       # Test dosyaları oluştur
```

## Kullanım

```bash
# Arşiv oluştur
./bin/tarsau -b dosya1.txt dosya2.txt -o arsiv.sau

# Arşivden çıkar
./bin/tarsau -a arsiv.sau

# Yardım
./bin/tarsau -h
```

## Teknolojiler

- **Dil:** C (C11 standardı)
- **Derleyici:** GCC
- **Yapı sistemi:** GNU Make
- **Platform:** Linux/Ubuntu (WSL2)
- **Sistem çağrıları:** open, read, write, close, stat, chmod, mkdir

## Lisans

Bu proje eğitim amaçlıdır.
