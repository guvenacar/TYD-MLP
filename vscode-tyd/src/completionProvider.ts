import * as vscode from 'vscode';

export class TydCompletionProvider implements vscode.CompletionItemProvider {
    provideCompletionItems(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken,
        context: vscode.CompletionContext
    ): vscode.ProviderResult<vscode.CompletionItem[] | vscode.CompletionList> {

        const completionItems: vscode.CompletionItem[] = [];

        // Tip Tanımları (hem büyük hem küçük harf)
        const typeKeywords = [
            { label: 'SAYISAL', detail: 'Sayısal tip (integer/float)', docs: 'Tam sayı veya ondalıklı sayı değişkeni tanımlar' },
            { label: 'sayısal', detail: 'Sayısal tip (integer/float)', docs: 'Tam sayı veya ondalıklı sayı değişkeni tanımlar' },
            { label: 'METIN', detail: 'Metin tipi (string)', docs: 'Metin (string) değişkeni tanımlar' },
            { label: 'metin', detail: 'Metin tipi (string)', docs: 'Metin (string) değişkeni tanımlar' },
            { label: 'ZITLIK', detail: 'Boolean tipi', docs: 'DOĞRU veya YANLIŞ değeri alan boolean değişken tanımlar' },
            { label: 'zıtlık', detail: 'Boolean tipi', docs: 'DOĞRU veya YANLIŞ değeri alan boolean değişken tanımlar' },
            { label: 'NESNE', detail: 'Nesne/Dictionary tipi', docs: 'Nesne veya sözlük yapısı tanımlar' },
            { label: 'nesne', detail: 'Nesne/Dictionary tipi', docs: 'Nesne veya sözlük yapısı tanımlar' }
        ];

        typeKeywords.forEach(kw => {
            const item = new vscode.CompletionItem(kw.label, vscode.CompletionItemKind.Keyword);
            item.detail = kw.detail;
            item.documentation = new vscode.MarkdownString(kw.docs);
            item.sortText = '0_' + kw.label; // Öncelikli göster
            completionItems.push(item);
        });

        // Kontrol Yapıları (hem büyük hem küçük harf)
        const controlKeywords = [
            {
                label: 'EĞER',
                detail: 'Koşul yapısı başlangıcı',
                docs: 'Koşullu ifade tanımlar\n\n**Örnek:**\n```tyd\nEĞER x > 5 İSE\n    YAZDIR "Büyük";\nEĞER SON\n```',
                insertText: 'EĞER ${1:kosul} İSE\n    ${2}\nEĞER SON'
            },
            {
                label: 'eğer',
                detail: 'Koşul yapısı başlangıcı',
                docs: 'Koşullu ifade tanımlar\n\n**Örnek:**\n```tyd\neğer x > 5 ise\n    yazdır "Büyük";\neğer son\n```',
                insertText: 'eğer ${1:kosul} ise\n    ${2}\neğer son'
            },
            {
                label: 'İSE',
                detail: 'Koşul yapısı devamı',
                docs: 'EĞER ifadesinden sonra gelir'
            },
            {
                label: 'ise',
                detail: 'Koşul yapısı devamı',
                docs: 'eğer ifadesinden sonra gelir'
            },
            {
                label: 'DEĞILSE',
                detail: 'Else bloğu',
                docs: 'Koşul yanlış olduğunda çalışacak bloğu tanımlar'
            },
            {
                label: 'değilse',
                detail: 'Else bloğu',
                docs: 'Koşul yanlış olduğunda çalışacak bloğu tanımlar'
            },
            {
                label: 'DÖNGÜ',
                detail: 'Döngü yapısı',
                docs: 'Sonsuz döngü tanımlar. DÖNGÜ_BITIR ile çıkılır\n\n**Örnek:**\n```tyd\nSAYISAL i = 0;\nDÖNGÜ\n    YAZDIR i;\n    i = i + 1;\n    EĞER i > 10 İSE\n        DÖNGÜ_BITIR;\n    EĞER SON\nDÖNGÜ SON\n```',
                insertText: 'DÖNGÜ\n    ${1}\n    \n    EĞER ${2:kosul} İSE\n        DÖNGÜ_BITIR;\n    EĞER SON\nDÖNGÜ SON'
            },
            {
                label: 'döngü',
                detail: 'Döngü yapısı',
                docs: 'Sonsuz döngü tanımlar. döngü_bitir ile çıkılır\n\n**Örnek:**\n```tyd\nsayısal i = 0;\ndöngü\n    yazdır i;\n    i = i + 1\n    eğer i > 10 ise\n        döngü_bitir;\n    eğer son\ndöngü son\n```',
                insertText: 'döngü\n    ${1}\n    \n    eğer ${2:kosul} ise\n        döngü_bitir;\n    eğer son\ndöngü son'
            },
            {
                label: 'DÖNGÜ_BITIR',
                detail: 'Döngüden çık',
                docs: 'Mevcut döngüyü sonlandırır (break)'
            },
            {
                label: 'döngü_bitir',
                detail: 'Döngüden çık',
                docs: 'Mevcut döngüyü sonlandırır (break)'
            },
            {
                label: 'SON',
                detail: 'Blok sonu',
                docs: 'EĞER, DÖNGÜ veya IŞLEÇ bloklarını kapatır'
            },
            {
                label: 'son',
                detail: 'Blok sonu',
                docs: 'eğer, döngü veya işleç bloklarını kapatır'
            }
        ];

        controlKeywords.forEach(kw => {
            const item = new vscode.CompletionItem(kw.label, vscode.CompletionItemKind.Keyword);
            item.detail = kw.detail;
            item.documentation = new vscode.MarkdownString(kw.docs);
            item.sortText = '2_' + kw.label; // Öncelikli göster
            if (kw.insertText) {
                item.insertText = new vscode.SnippetString(kw.insertText);
            }
            completionItems.push(item);
        });

        // Fonksiyon ve I/O (hem büyük hem küçük harf)
        const functionKeywords = [
            {
                label: 'IŞLEÇ',
                detail: 'Fonksiyon tanımlama',
                docs: 'Fonksiyon (işleç) tanımlar\n\n**Örnek:**\n```tyd\nSAYISAL IŞLEÇ topla(SAYISAL a, SAYISAL b)\n    DÖNÜŞ a + b;\nIŞLEÇ SON\n```',
                insertText: 'SAYISAL IŞLEÇ ${1:fonksiyon_adi}(${2:parametreler})\n    ${3}\n    DÖNÜŞ ${4:deger};\nIŞLEÇ SON'
            },
            {
                label: 'işleç',
                detail: 'Fonksiyon tanımlama',
                docs: 'Fonksiyon (işleç) tanımlar\n\n**Örnek:**\n```tyd\nsayısal işleç topla(sayısal a, sayısal b)\n    dönüş a + b;\nişleç son\n```',
                insertText: 'sayısal işleç ${1:fonksiyon_adi}(${2:parametreler})\n    ${3}\n    dönüş ${4:deger};\nişleç son'
            },
            {
                label: 'DÖNÜŞ',
                detail: 'Return ifadesi',
                docs: 'Fonksiyondan değer döndürür',
                insertText: 'DÖNÜŞ ${1:deger};'
            },
            {
                label: 'dönüş',
                detail: 'Return ifadesi',
                docs: 'Fonksiyondan değer döndürür',
                insertText: 'dönüş ${1:deger};'
            },
            {
                label: 'YAZDIR',
                detail: 'Çıktı yazdırma',
                docs: 'Değeri konsola yazdırır\n\n**Örnek:**\n```tyd\nYAZDIR "Merhaba";\nYAZDIR x;\n```',
                insertText: 'YAZDIR ${1:deger};'
            },
            {
                label: 'yazdır',
                detail: 'Çıktı yazdırma',
                docs: 'Değeri konsola yazdırır\n\n**Örnek:**\n```tyd\nyazdır "Merhaba";\nyazdır x;\n```',
                insertText: 'yazdır ${1:deger};'
            }
        ];

        functionKeywords.forEach(kw => {
            const item = new vscode.CompletionItem(kw.label, vscode.CompletionItemKind.Function);
            item.detail = kw.detail;
            item.documentation = new vscode.MarkdownString(kw.docs);
            item.sortText = '1_' + kw.label; // Öncelikli göster
            if (kw.insertText) {
                item.insertText = new vscode.SnippetString(kw.insertText);
            }
            completionItems.push(item);
        });

        // Sabitler
        const constants = [
            { label: 'DOĞRU', detail: 'Boolean true', docs: 'Mantıksal doğru değeri (1)' },
            { label: 'YANLIŞ', detail: 'Boolean false', docs: 'Mantıksal yanlış değeri (0)' },
            { label: 'HİÇLİK', detail: 'Null değeri', docs: 'Null/hiçlik değeri' }
        ];

        constants.forEach(c => {
            const item = new vscode.CompletionItem(c.label, vscode.CompletionItemKind.Constant);
            item.detail = c.detail;
            item.documentation = new vscode.MarkdownString(c.docs);
            completionItems.push(item);
        });

        // Operatörler
        const operators = [
            { label: '==', detail: 'Eşitlik kontrolü', docs: 'İki değerin eşit olup olmadığını kontrol eder' },
            { label: '>', detail: 'Büyüktür', docs: 'Soldaki değerin sağdakinden büyük olup olmadığını kontrol eder' },
            { label: '<', detail: 'Küçüktür', docs: 'Soldaki değerin sağdakinden küçük olup olmadığını kontrol eder' },
            { label: '>=', detail: 'Büyük veya eşit', docs: 'Soldaki değerin sağdakinden büyük veya eşit olup olmadığını kontrol eder' },
            { label: '<=', detail: 'Küçük veya eşit', docs: 'Soldaki değerin sağdakinden küçük veya eşit olup olmadığını kontrol eder' },
            { label: '**', detail: 'Üs alma', docs: 'Üs alma operatörü (x ** y = x^y)' }
        ];

        operators.forEach(op => {
            const item = new vscode.CompletionItem(op.label, vscode.CompletionItemKind.Operator);
            item.detail = op.detail;
            item.documentation = new vscode.MarkdownString(op.docs);
            completionItems.push(item);
        });

        return completionItems;
    }
}
