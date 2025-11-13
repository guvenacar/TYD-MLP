import * as vscode from 'vscode';
import { TydCompletionProvider } from './completionProvider';
import { TydHoverProvider } from './hoverProvider';
import { TydDiagnostics } from './diagnostics';

export function activate(context: vscode.ExtensionContext) {
    console.log('TYD Language Extension aktifleştirildi!');

    const tydSelector: vscode.DocumentSelector = { scheme: 'file', language: 'tyd' };

    // Completion Provider (IntelliSense)
    const completionProvider = vscode.languages.registerCompletionItemProvider(
        tydSelector,
        new TydCompletionProvider(),
        '.', ' '
    );

    // Hover Provider (Tooltips)
    const hoverProvider = vscode.languages.registerHoverProvider(
        tydSelector,
        new TydHoverProvider()
    );

    // Diagnostics (Hata Kontrolü)
    const diagnostics = new TydDiagnostics();

    // Dosya açıldığında veya değiştiğinde hata kontrolü yap
    const diagnosticsDisposable = vscode.workspace.onDidChangeTextDocument(event => {
        if (event.document.languageId === 'tyd') {
            diagnostics.updateDiagnostics(event.document);
        }
    });

    // Dosya kaydedildiğinde de kontrol et
    const saveDisposable = vscode.workspace.onDidSaveTextDocument(document => {
        if (document.languageId === 'tyd') {
            diagnostics.updateDiagnostics(document);
        }
    });

    // Aktif düzenleyici değiştiğinde kontrol et
    const editorDisposable = vscode.window.onDidChangeActiveTextEditor(editor => {
        if (editor && editor.document.languageId === 'tyd') {
            diagnostics.updateDiagnostics(editor.document);
        }
    });

    // İlk açıldığında mevcut belgeyi kontrol et
    if (vscode.window.activeTextEditor?.document.languageId === 'tyd') {
        diagnostics.updateDiagnostics(vscode.window.activeTextEditor.document);
    }

    context.subscriptions.push(
        completionProvider,
        hoverProvider,
        diagnosticsDisposable,
        saveDisposable,
        editorDisposable
    );
}

export function deactivate() {
    console.log('TYD Language Extension devre dışı bırakıldı.');
}
