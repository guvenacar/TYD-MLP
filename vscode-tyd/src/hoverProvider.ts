import * as vscode from 'vscode';

export class TydHoverProvider implements vscode.HoverProvider {
    private keywords: Map<string, { description: string, example?: string }>;

    constructor() {
        this.keywords = new Map();
        this.initializeKeywords();
    }

    private initializeKeywords() {
        // Tip Tanımları
        this.keywords.set('SAYISAL', {
            description: '**Sayısal Tip**\n\nTam sayı veya ondalıklı sayı değişkeni tanımlar.',
            example: 'SAYISAL x = 10;\nSAYISAL pi = 3.14;'
        });

        this.keywords.set('METIN', {
            description: '**Metin Tipi**\n\nString (metin) değişkeni tanımlar.',
            example: 'METIN isim = "Ali";\nMETIN mesaj = "Merhaba Dünya";'
        });

        this.keywords.set('ZITLIK', {
            description: '**Boolean Tipi**\n\nDOĞRU (1) veya YANLIŞ (0) değeri alan mantıksal değişken tanımlar.',
            example: 'ZITLIK aktif = DOĞRU;\nZITLIK tamamlandi = YANLIŞ;'
        });

        this.keywords.set('NESNE', {
            description: '**Nesne/Dictionary Tipi**\n\nNesne veya sözlük (dictionary) yapısı tanımlar.',
            example: 'NESNE kullanici = {"isim": "Ali", "yas": 25};'
        });

        // Kontrol Yapıları
        this.keywords.set('EĞER', {
            description: '**Koşul İfadesi**\n\nKoşullu dallanma için kullanılır. İSE ve SON ile birlikte kullanılır.',
            example: 'EĞER x > 5 İSE\n    YAZDIR "Büyük";\nEĞER SON'
        });

        this.keywords.set('İSE', {
            description: '**Koşul Devamı**\n\nEĞER ifadesinden sonra koşul bloğunu başlatır.',
            example: 'EĞER x > 5 İSE\n    YAZDIR "Büyük";\nEĞER SON'
        });

        this.keywords.set('DEĞILSE', {
            description: '**Else Bloğu**\n\nKoşul yanlış olduğunda çalışacak kod bloğunu tanımlar.',
            example: 'EĞER x > 5 İSE\n    YAZDIR "Büyük";\nDEĞİLSE\n    YAZDIR "Küçük";\nEĞER SON'
        });

        this.keywords.set('DÖNGÜ', {
            description: '**Döngü Yapısı**\n\nSonsuz döngü tanımlar. DÖNGÜ_BITIR ile çıkılır.',
            example: 'SAYISAL i = 0;\nDÖNGÜ\n    YAZDIR i;\n    i = i + 1;\n    EĞER i > 10 İSE\n        DÖNGÜ_BITIR;\n    EĞER SON\nDÖNGÜ SON'
        });

        this.keywords.set('DÖNGÜ_BITIR', {
            description: '**Döngü Kırma**\n\nMevcut döngüyü sonlandırır (break komutu).',
            example: 'EĞER x > 100 İSE\n    DÖNGÜ_BITIR;\nEĞER SON'
        });

        this.keywords.set('SON', {
            description: '**Blok Sonu**\n\nEĞER, DÖNGÜ veya IŞLEÇ bloklarını kapatır.',
            example: 'EĞER x > 5 İSE\n    YAZDIR "OK";\nEĞER SON  -- Blok sonu'
        });

        // Fonksiyon ve I/O
        this.keywords.set('IŞLEÇ', {
            description: '**Fonksiyon Tanımlama**\n\nYeniden kullanılabilir kod bloğu (fonksiyon) tanımlar.',
            example: 'SAYISAL IŞLEÇ topla(SAYISAL a, SAYISAL b)\n    DÖNÜŞ a + b;\nIŞLEÇ SON'
        });

        this.keywords.set('DÖNÜŞ', {
            description: '**Return İfadesi**\n\nFonksiyondan değer döndürür.',
            example: 'SAYISAL IŞLEÇ kare(SAYISAL x)\n    DÖNÜŞ x * x;\nIŞLEÇ SON'
        });

        this.keywords.set('YAZDIR', {
            description: '**Çıktı Yazdırma**\n\nDeğeri konsola/ekrana yazdırır.',
            example: 'YAZDIR "Merhaba";\nYAZDIR x;\nYAZDIR 42;'
        });

        // Sabitler
        this.keywords.set('DOĞRU', {
            description: '**Boolean True**\n\nMantıksal doğru değeri (1)',
            example: 'ZITLIK aktif = DOĞRU;'
        });

        this.keywords.set('YANLIŞ', {
            description: '**Boolean False**\n\nMantıksal yanlış değeri (0)',
            example: 'ZITLIK tamamlandi = YANLIŞ;'
        });

        this.keywords.set('HİÇLİK', {
            description: '**Null Değeri**\n\nHiçlik/boş değer',
            example: 'NESNE obj = HİÇLİK;'
        });

        // Operatörler
        this.keywords.set('==', {
            description: '**Eşitlik Kontrolü**\n\nİki değerin eşit olup olmadığını kontrol eder.',
            example: 'EĞER x == 5 İSE\n    YAZDIR "Eşit";\nEĞER SON'
        });

        this.keywords.set('**', {
            description: '**Üs Alma Operatörü**\n\nBir sayının üssünü alır (x ** y = x^y)',
            example: 'SAYISAL kare = 2 ** 2;  -- 4\nSAYISAL kup = 2 ** 3;   -- 8'
        });
    }

    provideHover(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.Hover> {

        const range = document.getWordRangeAtPosition(position);
        if (!range) {
            return null;
        }

        const word = document.getText(range).toUpperCase();

        // Anahtar kelime kontrolü
        const keywordInfo = this.keywords.get(word);
        if (keywordInfo) {
            const markdown = new vscode.MarkdownString();
            markdown.appendMarkdown(keywordInfo.description);

            if (keywordInfo.example) {
                markdown.appendMarkdown('\n\n**Örnek:**\n```tyd\n' + keywordInfo.example + '\n```');
            }

            return new vscode.Hover(markdown);
        }

        // Operatör kontrolü (== ve ** gibi)
        const lineText = document.lineAt(position.line).text;
        const charAtPos = lineText.charAt(position.character);

        if (charAtPos === '=' && position.character + 1 < lineText.length) {
            const nextChar = lineText.charAt(position.character + 1);
            if (nextChar === '=') {
                const info = this.keywords.get('==');
                if (info) {
                    const markdown = new vscode.MarkdownString();
                    markdown.appendMarkdown(info.description);
                    if (info.example) {
                        markdown.appendMarkdown('\n\n**Örnek:**\n```tyd\n' + info.example + '\n```');
                    }
                    return new vscode.Hover(markdown);
                }
            }
        }

        if (charAtPos === '*' && position.character + 1 < lineText.length) {
            const nextChar = lineText.charAt(position.character + 1);
            if (nextChar === '*') {
                const info = this.keywords.get('**');
                if (info) {
                    const markdown = new vscode.MarkdownString();
                    markdown.appendMarkdown(info.description);
                    if (info.example) {
                        markdown.appendMarkdown('\n\n**Örnek:**\n```tyd\n' + info.example + '\n```');
                    }
                    return new vscode.Hover(markdown);
                }
            }
        }

        return null;
    }
}
