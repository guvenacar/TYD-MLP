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

            // Yorumları atla
            if (line.startsWith('--')) {
                continue;
            }

            // Blok başlangıçlarını bul
            for (const keyword of blockKeywords) {
                if (line.includes(keyword) && !line.includes(keyword + ' SON')) {
                    blockStarts.push({ keyword, line: i });
                }
            }

            // SON kontrolü
            if (line.includes('EĞER SON') || line.includes('DÖNGÜ SON') || line.includes('IŞLEÇ SON')) {
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
        const needsSemicolon = ['YAZDIR', 'SAYISAL', 'METIN', 'ZITLIK', 'NESNE', 'DÖNÜŞ', 'DÖNGÜ_BITIR'];

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i].trim();

            // Yorumları ve boş satırları atla
            if (line.startsWith('--') || line.length === 0) {
                continue;
            }

            // SON veya blok başlangıçları noktalı virgül gerektirmez
            if (line.includes('SON') || line.includes('İSE') || line.includes('DEĞILSE') ||
                line === 'DÖNGÜ' || line.endsWith('IŞLEÇ')) {
                continue;
            }

            // Noktalı virgül gerektiren ifadeleri kontrol et
            for (const keyword of needsSemicolon) {
                if (line.includes(keyword) && !line.endsWith(';')) {
                    const range = new vscode.Range(i, 0, i, line.length);
                    const diagnostic = new vscode.Diagnostic(
                        range,
                        `Satır sonu noktalı virgül (;) ile bitmeli`,
                        vscode.DiagnosticSeverity.Warning
                    );
                    diagnostics.push(diagnostic);
                    break;
                }
            }

            // Atama ifadeleri
            if (line.includes('=') && !line.includes('==') && !line.includes('>') &&
                !line.includes('<') && !line.includes('IŞLEÇ') && !line.endsWith(';')) {
                const range = new vscode.Range(i, 0, i, line.length);
                const diagnostic = new vscode.Diagnostic(
                    range,
                    'Atama ifadesi noktalı virgül (;) ile bitmeli',
                    vscode.DiagnosticSeverity.Warning
                );
                diagnostics.push(diagnostic);
            }
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

            // Yorumları atla
            if (line.startsWith('--')) {
                continue;
            }

            // EĞER kullanımı - İSE olmadan kullanılmış mı?
            if (line.includes('EĞER') && !line.includes('İSE') && !line.includes('EĞER SON')) {
                // Bir sonraki satırda İSE var mı kontrol et
                if (i + 1 >= lines.length || !lines[i + 1].trim().includes('İSE')) {
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
            if (line.includes('DÖNÜŞ') && line.includes('IŞLEÇ') === false) {
                // DÖNÜŞ sadece fonksiyon içinde olmalı
                let inFunction = false;
                for (let j = i - 1; j >= 0; j--) {
                    if (lines[j].includes('IŞLEÇ') && !lines[j].includes('IŞLEÇ SON')) {
                        inFunction = true;
                        break;
                    }
                    if (lines[j].includes('IŞLEÇ SON')) {
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
