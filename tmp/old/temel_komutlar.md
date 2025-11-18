İşte Python, JavaScript, C# ve Java dillerinde temel komutlar:

## 🔄 FOR DÖNGÜSÜ

### Python
```python
# Range ile
for i in range(5):
    print(i)

# Liste ile
meyveler = ["elma", "armut", "çilek"]
for meyve in meyveler:
    print(meyve)

# Dictionary ile
ogrenci = {"ad": "Ali", "yas": 20}
for anahtar, deger in ogrenci.items():
    print(f"{anahtar}: {deger}")
```

### JavaScript
```javascript
// Klasik for
for (let i = 0; i < 5; i++) {
    console.log(i);
}

// For...of (diziler için)
const meyveler = ["elma", "armut", "çilek"];
for (const meyve of meyveler) {
    console.log(meyve);
}

// For...in (objeler için)
const ogrenci = {ad: "Ali", yas: 20};
for (const anahtar in ogrenci) {
    console.log(`${anahtar}: ${ogrenci[anahtar]}`);
}
```

### C#
```csharp
// Klasik for
for (int i = 0; i < 5; i++)
{
    Console.WriteLine(i);
}

// Foreach
string[] meyveler = {"elma", "armut", "çilek"};
foreach (string meyve in meyveler)
{
    Console.WriteLine(meyve);
}
```

### Java
```java
// Klasik for
for (int i = 0; i < 5; i++) {
    System.out.println(i);
}

// Foreach
String[] meyveler = {"elma", "armut", "çilek"};
for (String meyve : meyveler) {
    System.out.println(meyve);
}
```

## ⚡ IF KOMUTU

### Python
```python
yas = 18

if yas >= 18:
    print("Reşitsiniz")
elif yas >= 13:
    print("Gençsiniz")
else:
    print("Çocuksunuz")

# Kısa if
durum = "Reşit" if yas >= 18 else "Reşit değil"
```

### JavaScript
```javascript
let yas = 18;

if (yas >= 18) {
    console.log("Reşitsiniz");
} else if (yas >= 13) {
    console.log("Gençsiniz");
} else {
    console.log("Çocuksunuz");
}

// Ternary operator
let durum = yas >= 18 ? "Reşit" : "Reşit değil";
```

### C#
```csharp
int yas = 18;

if (yas >= 18)
{
    Console.WriteLine("Reşitsiniz");
}
else if (yas >= 13)
{
    Console.WriteLine("Gençsiniz");
}
else
{
    Console.WriteLine("Çocuksunuz");
}

// Ternary operator
string durum = yas >= 18 ? "Reşit" : "Reşit değil";
```

### Java
```java
int yas = 18;

if (yas >= 18) {
    System.out.println("Reşitsiniz");
} else if (yas >= 13) {
    System.out.println("Gençsiniz");
} else {
    System.out.println("Çocuksunuz");
}

// Ternary operator
String durum = yas >= 18 ? "Reşit" : "Reşit değil";
```

## 🔄 WHILE DÖNGÜSÜ

### Python
```python
# While
sayac = 0
while sayac < 5:
    print(sayac)
    sayac += 1

# Do-while benzeri (Python'da do-while yok)
while True:
    kullanici_girdi = input("Devam etmek için 'e' yazın: ")
    if kullanici_girdi != 'e':
        break
```

### JavaScript
```javascript
// While
let sayac = 0;
while (sayac < 5) {
    console.log(sayac);
    sayac++;
}

// Do-while
let sayac2 = 0;
do {
    console.log(sayac2);
    sayac2++;
} while (sayac2 < 5);
```

### C#
```csharp
// While
int sayac = 0;
while (sayac < 5)
{
    Console.WriteLine(sayac);
    sayac++;
}

// Do-while
int sayac2 = 0;
do
{
    Console.WriteLine(sayac2);
    sayac2++;
} while (sayac2 < 5);
```

### Java
```java
// While
int sayac = 0;
while (sayac < 5) {
    System.out.println(sayac);
    sayac++;
}

// Do-while
int sayac2 = 0;
do {
    System.out.println(sayac2);
    sayac2++;
} while (sayac2 < 5);
```

## 📋 SWITCH-CASE

### Python (match-case - Python 3.10+)
```python
gun = "pazartesi"

match gun:
    case "pazartesi":
        print("Haftanın ilk günü")
    case "cuma":
        print("Hafta sonu yakın")
    case _:
        print("Diğer gün")
```

### JavaScript
```javascript
const gun = "pazartesi";

switch (gun) {
    case "pazartesi":
        console.log("Haftanın ilk günü");
        break;
    case "cuma":
        console.log("Hafta sonu yakın");
        break;
    default:
        console.log("Diğer gün");
}
```

### C#
```csharp
string gun = "pazartesi";

switch (gun)
{
    case "pazartesi":
        Console.WriteLine("Haftanın ilk günü");
        break;
    case "cuma":
        Console.WriteLine("Hafta sonu yakın");
        break;
    default:
        Console.WriteLine("Diğer gün");
        break;
}
```

### Java
```java
String gun = "pazartesi";

switch (gun) {
    case "pazartesi":
        System.out.println("Haftanın ilk günü");
        break;
    case "cuma":
        System.out.println("Hafta sonu yakın");
        break;
    default:
        System.out.println("Diğer gün");
        break;
}
```

## 🎯 FONKSİYON TANIMLAMA

### Python
```python
def topla(a, b):
    return a + b

# Lambda fonksiyon
carp = lambda a, b: a * b
```

### JavaScript
```javascript
// Function declaration
function topla(a, b) {
    return a + b;
}

// Arrow function
const carp = (a, b) => a * b;
```

### C#
```csharp
public int Topla(int a, int b)
{
    return a + b;
}

// Lambda expression
Func<int, int, int> carp = (a, b) => a * b;
```

### Java
```java
public int topla(int a, int b) {
    return a + b;
}

// Lambda expression (Java 8+)
BiFunction<Integer, Integer, Integer> carp = (a, b) -> a * b;
```

Bu temel komutlar her programlama dilinde benzer mantıkla çalışır, sadece syntax (yazım) farklılıkları vardır.