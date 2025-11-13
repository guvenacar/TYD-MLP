# TYD VSCode Extension - Geliştirici Kılavuzu

Bu belge, TYD VSCode eklentisinin geliştirilmesi ve test edilmesi için gerekli bilgileri içerir.

## Gereksinimler

- Node.js 16 veya üzeri
- npm 7 veya üzeri
- VSCode 1.75 veya üzeri
- TypeScript 4.9 veya üzeri

## Kurulum

1. Depoyu klonlayın:
```bash
git clone https://github.com/guvenacar/TYD-MLP.git
cd TYD-MLP/vscode-tyd
```

2. Bağımlılıkları yükleyin:
```bash
npm install
```

3. TypeScript derleyicisini çalıştırın:
```bash
npm run compile
```

## Geliştirme

### Watch Mode
Geliştirme sırasında otomatik derleme için:
```bash
npm run watch
```

### Debug/Test
1. VSCode'da `vscode-tyd` klasörünü açın
2. F5 tuşuna basın
3. Yeni bir Extension Development Host penceresi açılacak
4. Bu pencerede `.tyd` uzantılı bir dosya açın
5. Değişiklikleri test edin

### Kod Yapısı

```
vscode-tyd/
├── src/
│   ├── extension.ts           # Ana eklenti dosyası
│   ├── completionProvider.ts  # IntelliSense kod tamamlama
│   ├── hoverProvider.ts       # Hover tooltips
│   └── diagnostics.ts         # Sözdizimi kontrolü
├── syntaxes/
│   └── tyd.tmLanguage.json   # Syntax highlighting
├── snippets/
│   └── tyd.json              # Code snippets
├── language-configuration.json # Dil konfigürasyonu
├── package.json               # Eklenti manifest
└── tsconfig.json             # TypeScript config
```

## Eklenti Bileşenleri

### 1. Extension.ts
Ana eklenti giriş noktası. Tüm provider'ları kaydeder ve yaşam döngüsünü yönetir.

**Önemli Fonksiyonlar:**
- `activate()`: Eklenti aktif olduğunda çağrılır
- `deactivate()`: Eklenti devre dışı bırakıldığında çağrılır

### 2. CompletionProvider.ts
IntelliSense kod tamamlama önerileri sağlar.

**Özellikler:**
- Anahtar kelime tamamlama
- Snippet'li öneriler
- Detaylı açıklamalar
- Örnek kodlar

### 3. HoverProvider.ts
Kod üzerine gelindiğinde tooltip gösterir.

**Özellikler:**
- Anahtar kelime açıklamaları
- Kullanım örnekleri
- Markdown formatında dokümantasyon

### 4. Diagnostics.ts
Gerçek zamanlı sözdizimi kontrolü yapar.

**Kontroller:**
- Blok yapısı (EĞER-SON, DÖNGÜ-SON)
- Noktalı virgül kontrolü
- String kapatma kontrolü
- Anahtar kelime kullanım kontrolü

### 5. tyd.tmLanguage.json
TextMate syntax highlighting tanımları.

**Tanımlanan Scope'lar:**
- `keyword.control.tyd` - Kontrol yapıları
- `storage.type.tyd` - Tip tanımları
- `string.quoted.double.tyd` - Stringler
- `constant.numeric.tyd` - Sayılar
- `comment.line.tyd` - Yorumlar

## Test Etme

### Manuel Test
1. F5 ile Extension Development Host'u başlatın
2. Yeni bir dosya oluşturun: `test.tyd`
3. Aşağıdaki özellikleri test edin:

**Syntax Highlighting:**
```tyd
SAYISAL x = 10;  -- Anahtar kelimeler renkli mi?
METIN ad = "Test";  -- String vurgulanıyor mu?
```

**IntelliSense:**
- `SAY` yazıp Ctrl+Space
- SAYISAL önerisi geldi mi?

**Hover:**
- `EĞER` kelimesinin üzerine gelin
- Açıklama görünüyor mu?

**Diagnostics:**
```tyd
EĞER x > 5 İSE
    YAZDIR "Test"  -- Noktalı virgül eksik, uyarı var mı?
-- EĞER SON eksik, hata var mı?
```

### Otomatik Test (İleride Eklenebilir)
```bash
npm run test
```

## Paketleme

Eklentiyi `.vsix` dosyası olarak paketlemek için:

1. `vsce` aracını yükleyin:
```bash
npm install -g @vscode/vsce
```

2. Paketi oluşturun:
```bash
npm run package
```

3. Oluşan `.vsix` dosyasını yükleyin:
```bash
code --install-extension tyd-language-0.1.0.vsix
```

## Yayınlama

### VSCode Marketplace'e Yayınlama

1. [Azure DevOps](https://dev.azure.com) hesabı oluşturun
2. Personal Access Token alın
3. Publisher oluşturun:
```bash
vsce create-publisher <publisher-name>
```

4. Login olun:
```bash
vsce login <publisher-name>
```

5. Yayınlayın:
```bash
vsce publish
```

## Hata Ayıklama İpuçları

### Eklenti Yüklenmiyor
- `package.json` içinde `activationEvents` doğru mu?
- TypeScript derlemesi başarılı mı? (`npm run compile`)

### IntelliSense Çalışmıyor
- Extension Development Host'ta "Developer: Reload Window" yapın
- Console'da hata mesajı var mı?

### Syntax Highlighting Yanlış
- `tyd.tmLanguage.json` içindeki regex pattern'leri kontrol edin
- VSCode'u yeniden başlatın

### Diagnostics Çalışmıyor
- `diagnostics.ts` içindeki `updateDiagnostics` çağrılıyor mu?
- Document listener'lar doğru kayıtlı mı?

## Katkıda Bulunma

1. Yeni özellik eklerken:
   - Önce bir issue açın
   - Test ekleyin
   - Dokümantasyonu güncelleyin

2. Kod stili:
   - TypeScript strict mode kullanın
   - ESLint kurallarına uyun
   - Anlamlı değişken isimleri kullanın

3. Commit mesajları:
   - Türkçe veya İngilizce
   - Açıklayıcı olsun
   - Örnek: "feat: NESNE tipi için IntelliSense desteği eklendi"

## Kaynaklar

- [VSCode Extension API](https://code.visualstudio.com/api)
- [Language Extensions](https://code.visualstudio.com/api/language-extensions/overview)
- [TextMate Grammars](https://macromates.com/manual/en/language_grammars)
- [Extension Guidelines](https://code.visualstudio.com/api/references/extension-guidelines)

## İletişim

Sorularınız için:
- GitHub Issues: https://github.com/guvenacar/TYD-MLP/issues
- Email: guven.acar@gmail.com
