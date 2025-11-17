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

    // ============================================
    // BUILD KOMUTLARI
    // ============================================

    // TYD: Compile Current File
    const compileCommand = vscode.commands.registerCommand('tyd.compile', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'tyd') {
            vscode.window.showErrorMessage('Lütfen bir TYD dosyası açın!');
            return;
        }

        const filePath = editor.document.fileName;
        const terminal = vscode.window.createTerminal('TYD Compiler');
        terminal.show();

        // C compiler yolunu bul
        const workspaceRoot = vscode.workspace.workspaceFolders?.[0].uri.fsPath;
        const compilerPath = workspaceRoot + '/c_compiler/compiler_test';
        const outputPath = filePath.replace('.tyd', '.asm');

        terminal.sendText(`${compilerPath} "${filePath}" "${outputPath}"`);
        vscode.window.showInformationMessage(`TYD derleniyor: ${filePath}`);
    });

    // TYD: Compile and Run
    const compileAndRunCommand = vscode.commands.registerCommand('tyd.compileAndRun', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'tyd') {
            vscode.window.showErrorMessage('Lütfen bir TYD dosyası açın!');
            return;
        }

        const filePath = editor.document.fileName;
        const terminal = vscode.window.createTerminal('TYD Run');
        terminal.show();

        const workspaceRoot = vscode.workspace.workspaceFolders?.[0].uri.fsPath;
        const runScript = workspaceRoot + '/c_compiler/calistir.sh';

        terminal.sendText(`cd "${workspaceRoot}/c_compiler" && ./calistir.sh "${filePath}"`);
        vscode.window.showInformationMessage(`TYD çalıştırılıyor: ${filePath}`);
    });

    // TYD: Build All
    const buildAllCommand = vscode.commands.registerCommand('tyd.buildAll', async () => {
        const workspaceRoot = vscode.workspace.workspaceFolders?.[0].uri.fsPath;
        const terminal = vscode.window.createTerminal('TYD Build All');
        terminal.show();

        terminal.sendText(`cd "${workspaceRoot}" && find . -name "*.tyd" -exec echo "Compiling: {}" \\;`);
        vscode.window.showInformationMessage('Tüm TYD dosyaları derleniyor...');
    });

    context.subscriptions.push(
        completionProvider,
        hoverProvider,
        diagnosticsDisposable,
        saveDisposable,
        editorDisposable,
        compileCommand,
        compileAndRunCommand,
        buildAllCommand
    );
}

export function deactivate() {
    console.log('TYD Language Extension devre dışı bırakıldı.');
}
