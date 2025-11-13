import * as vscode from 'vscode';

export class TydDiagnostics {
    private diagnosticCollection: vscode.DiagnosticCollection;

    constructor() {
        this.diagnosticCollection = vscode.languages.createDiagnosticCollection('tyd');
    }

    public updateDiagnostics(document: vscode.TextDocument): void {
        if (document.languageId !== 'tyd') {
            return;
        }

        const diagnostics: vscode.Diagnostic[] = [];
        const text = document.getText();
        const lines = text.split('\n');

        // Basit sözdizimi kontrolleri
        this.checkBlockStructure(document, lines, diagnostics);
        this.checkSemicolons(document, lines, diagnostics);
        this.checkQuotes(document, lines, diagnostics);
        this.checkKeywordUsage(document, lines, diagnostics);

        this.diagnosticCollection.set(document.uri, diagnostics);
    }

    private checkBlockStructure(document: vscode.TextDocument, lines: string[], diagnostics: vscode.Diagnostic[]): void {
        const blockStarts: { keyword: string, line: number }[] = [];
        const blockKeywords = ['EĞER', 'DÖNGÜ', 'IŞLEÇ'];

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i].trim();
            const lineUpper = line.toUpperCase(); // Case-insensitive kontrol

            // Yorumları atla
            if (line.startsWith('--')) {
                continue;
            }

            // Blok başlangıçlarını bul
            for (const keyword of blockKeywords) {
                if (lineUpper.includes(keyword) && !lineUpper.includes(keyword + ' SON')) {
                    blockStarts.push({ keyword, line: i });
                }
            }

            // SON kontrolü
            if (lineUpper.includes('EĞER SON') || lineUpper.includes('DÖNGÜ SON') || lineUpper.includes('IŞLEÇ SON')) {
                if (blockStarts.length === 0) {
                    const range = new vscode.Range(i, 0, i, line.length);
                    const diagnostic = new vscode.Diagnostic(
                        range,
                        'Beklenmeyen "SON" - açılmamış bir blok kapatılıyor',
                        vscode.DiagnosticSeverity.Error
                    );
                    diagnostics.push(diagnostic);
                } else {
                    blockStarts.pop();
                }
            }
        }

        // Kapatılmamış bloklar
        for (const block of blockStarts) {
            const range = new vscode.Range(block.line, 0, block.line, lines[block.line].length);
            const diagnostic = new vscode.Diagnostic(
                range,
                `"${block.keyword}" bloğu kapatılmamış - "SON" eksik`,
                vscode.DiagnosticSeverity.Error
            );
            diagnostics.push(diagnostic);
        }
    }

    private checkSemicolons(document: vscode.TextDocument, lines: string[], diagnostics: vscode.Diagnostic[]): void {
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i].trim();
            const lineUpper = line.toUpperCase(); // Case-insensitive kontrol için

            // Yorumları ve boş satırları atla
            if (line.startsWith('--') || line.length === 0) {
                continue;
            }

            // SON veya blok başlangıçları noktalı virgül gerektirmez
            if (lineUpper.includes('SON') || lineUpper.includes('İSE') || lineUpper.includes('DEĞILSE') ||
                lineUpper === 'DÖNGÜ' || (lineUpper.includes('IŞLEÇ') && line.includes('('))) {
                continue;
            }

            // KURAL 1: Sadece İLK TANIM noktalı virgül gerektirir (SAYISAL/METIN/ZITLIK x = değer;)
            const isDeclaration = (lineUpper.startsWith('SAYISAL ') || lineUpper.startsWith('METIN ') ||
                                   lineUpper.startsWith('ZITLIK ') || lineUpper.startsWith('NESNE ')) && line.includes('=');

            // KURAL 2: Bu komutlar her zaman noktalı virgül gerektirir
            const alwaysNeedsSemicolon = ['YAZDIR', 'DÖNÜŞ', 'DÖNGÜ_BITIR'];
            const hasAlwaysSemicolonKeyword = alwaysNeedsSemicolon.some(kw => lineUpper.includes(kw));

            // Noktalı virgül kontrolü
            if ((isDeclaration || hasAlwaysSemicolonKeyword) && !line.endsWith(';')) {
                const range = new vscode.Range(i, 0, i, line.length);
                const diagnostic = new vscode.Diagnostic(
                    range,
                    isDeclaration ?
                        'Değişken tanımı noktalı virgül (;) ile bitmeli' :
                        'Bu komut noktalı virgül (;) ile bitmeli',
                    vscode.DiagnosticSeverity.Warning
                );
                diagnostics.push(diagnostic);
            }

            // NOT: Normal atamalar (x = 10, d = 12) artık noktalı virgül GEREKTIRMEZ
        }
    }

    private checkQuotes(document: vscode.TextDocument, lines: string[], diagnostics: vscode.Diagnostic[]): void {
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];

            // Yorumları atla
            const commentIndex = line.indexOf('--');
            const checkLine = commentIndex !== -1 ? line.substring(0, commentIndex) : line;

            // Tırnak işaretlerini say
            const quoteCount = (checkLine.match(/"/g) || []).length;

            if (quoteCount % 2 !== 0) {
                const range = new vscode.Range(i, 0, i, line.length);
                const diagnostic = new vscode.Diagnostic(
                    range,
                    'Kapatılmamış string - çift tırnak (") eksik',
                    vscode.DiagnosticSeverity.Error
                );
                diagnostics.push(diagnostic);
            }
        }
    }

    private checkKeywordUsage(document: vscode.TextDocument, lines: string[], diagnostics: vscode.Diagnostic[]): void {
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i].trim();
            const lineUpper = line.toUpperCase(); // Case-insensitive kontrol

            // Yorumları atla
            if (line.startsWith('--')) {
                continue;
            }

            // EĞER kullanımı - İSE olmadan kullanılmış mı?
            if (lineUpper.includes('EĞER') && !lineUpper.includes('İSE') && !lineUpper.includes('EĞER SON')) {
                // Bir sonraki satırda İSE var mı kontrol et
                if (i + 1 >= lines.length || !lines[i + 1].trim().toUpperCase().includes('İSE')) {
                    const range = new vscode.Range(i, 0, i, line.length);
                    const diagnostic = new vscode.Diagnostic(
                        range,
                        'EĞER ifadesinden sonra İSE gelmelidir',
                        vscode.DiagnosticSeverity.Error
                    );
                    diagnostics.push(diagnostic);
                }
            }

            // Tanımsız değişken kullanımı için basit kontrol
            // (Daha gelişmiş bir analiz için sembol tablosu gerekir)
            if (lineUpper.includes('DÖNÜŞ') && lineUpper.includes('IŞLEÇ') === false) {
                // DÖNÜŞ sadece fonksiyon içinde olmalı
                let inFunction = false;
                for (let j = i - 1; j >= 0; j--) {
                    const prevLineUpper = lines[j].toUpperCase();
                    if (prevLineUpper.includes('IŞLEÇ') && !prevLineUpper.includes('IŞLEÇ SON')) {
                        inFunction = true;
                        break;
                    }
                    if (prevLineUpper.includes('IŞLEÇ SON')) {
                        break;
                    }
                }

                if (!inFunction) {
                    const range = new vscode.Range(i, 0, i, line.length);
                    const diagnostic = new vscode.Diagnostic(
                        range,
                        'DÖNÜŞ ifadesi sadece fonksiyon (IŞLEÇ) içinde kullanılabilir',
                        vscode.DiagnosticSeverity.Error
                    );
                    diagnostics.push(diagnostic);
                }
            }
        }
    }

    public clear(): void {
        this.diagnosticCollection.clear();
    }

    public dispose(): void {
        this.diagnosticCollection.dispose();
    }
}
