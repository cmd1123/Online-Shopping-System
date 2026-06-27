#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

using namespace std;

class Product {
protected:
    string id;
    string name;
    double basePrice;
public:
    Product(string i, string n, double p) : id(i), name(n), basePrice(p) {}
    virtual ~Product() {}
    string getId() const { return id; }
    string getName() const { return name; }
    virtual double getPrice() const = 0;
    virtual void displayInfo() const = 0;
};

class ElectronicsProd : public Product {
private:
    int warrantyMonths;
public:
    ElectronicsProd(string i, string n, double p, int w) : Product(i, n, p), warrantyMonths(w) {}
    double getPrice() const override { return basePrice * 1.1; }
    void displayInfo() const override {
        cout << "[Dien tu] " << name << " (ID: " << id << ") - Gia goc: " << basePrice 
             << " USD (+10% VAT) -> " << getPrice() << " USD - BH: " << warrantyMonths << " thang";
    }
};

class ClothingProd : public Product {
private:
    string size;
public:
    ClothingProd(string i, string n, double p, string s) : Product(i, n, p), size(s) {}
    double getPrice() const override { return basePrice * 0.9; }
    void displayInfo() const override {
        cout << "[Thoi trang] " << name << " (ID: " << id << ") - Gia goc: " << basePrice 
             << " USD (Giam 10%) -> " << getPrice() << " USD - Size: " << size;
    }
};

class ProductStock {
private:
    shared_ptr<Product> product;
    int quantity;
public:
    ProductStock(shared_ptr<Product> p, int q) : product(p), quantity(q) {}
    shared_ptr<Product> getProduct() const { return product; }
    int getQuantity() const { return quantity; }
    void addQuantity(int q) { quantity += q; }
    bool removeQuantity(int q) {
        if (quantity >= q) {
            quantity -= q;
            return true;
        }
        return false;
    }
};

class Inventory {
private:
    vector<ProductStock> stocks;
public:
    void addProduct(shared_ptr<Product> p, int q) {
        for (auto& stock : stocks) {
            if (stock.getProduct()->getId() == p->getId()) {
                stock.addQuantity(q);
                return;
            }
        }
        stocks.push_back(ProductStock(p, q));
    }
    void viewInventory() const {
        cout << "\n--- DANH SACH SAN PHAM TRONG KHO ---" << endl;
        if (stocks.empty()) {
            cout << "Kho hang dang trong!" << endl;
            return;
        }
        for (const auto& stock : stocks) {
            stock.getProduct()->displayInfo();
            cout << " | So luong: " << stock.getQuantity() << endl;
        }
    }
    shared_ptr<Product> findProduct(string id) {
        for (auto& stock : stocks) {
            if (stock.getProduct()->getId() == id && stock.getQuantity() > 0) {
                return stock.getProduct();
            }
        }
        return nullptr;
    }
    bool deductStock(string id, int q) {
        for (auto& stock : stocks) {
            if (stock.getProduct()->getId() == id) {
                return stock.removeQuantity(q);
            }
        }
        return false;
    }
};

class OrderItem {
private:
    shared_ptr<Product> product;
    int quantity;
public:
    OrderItem(shared_ptr<Product> p, int q) : product(p), quantity(q) {}
    double SubTotal() const { return product->getPrice() * quantity; }
    void displayItem() const {
        cout << "  + " << product->getName() << " x" << quantity << " = " << SubTotal() << " USD" << endl;
    }
};

class PaymentStrategy {
public:
    virtual ~PaymentStrategy() {}
    virtual void pay(double amount) = 0;
};

class CreditCard : public PaymentStrategy {
private:
    string cardNumber;
public:
    CreditCard(string num) : cardNumber(num) {}
    void pay(double amount) override {
        cout << "Thanh toan " << amount << " USD thanh cong qua The Tin Dung (" << cardNumber << ")." << endl;
    }
};

class EWallet : public PaymentStrategy {
private:
    string phoneNumber;
public:
    EWallet(string phone) : phoneNumber(phone) {}
    void pay(double amount) override {
        cout << "Thanh toan " << amount << " USD thanh cong qua Vi Dien Tu (" << phoneNumber << ")." << endl;
    }
};

class Order {
private:
    string orderId;
    vector<OrderItem> items;
    string status;
public:
    Order(string id) : orderId(id), status("Cho thanh toan") {}
    void addItem(shared_ptr<Product> p, int q) {
        items.push_back(OrderItem(p, q));
    }
    double calculateTotal() const {
        double total = 0;
        for (const auto& item : items) {
            total += item.SubTotal();
        }
        return total;
    }
    bool isEmpty() const { return items.empty(); }
    void processPayment(unique_ptr<PaymentStrategy> paymentMethod) {
        if (items.empty()) {
            cout << "Don hang trong!" << endl;
            return;
        }
        double total = calculateTotal();
        paymentMethod->pay(total);
        status = "Da thanh toan - Dang chuan bi hang";
    }
    void displayOrder() const {
        cout << "Ma don hang: " << orderId << " | Trang thai: " << status << endl;
        for (const auto& item : items) {
            item.displayItem();
        }
        cout << "=> Tong cong: " << calculateTotal() << " USD" << endl;
    }
};

class User {
protected:
    string username;
    string password;
    string email;
public:
    User(string u, string p, string e) : username(u), password(p), email(e) {}
    virtual ~User() {}
    string getUsername() const { return username; }
    bool checkPassword(string p) const { return password == p; }
    virtual void displayRole() const = 0;
};

class Customer : public User {
private:
    vector<Order> orderHistory;
public:
    Customer(string u, string p, string e) : User(u, p, e) {}
    void displayRole() const override {
        cout << "Khach hang: " << username << " (" << email << ")" << endl;
    }
    void addOrder(const Order& o) {
        orderHistory.push_back(o);
    }
    void viewOrderHistory() const {
        cout << "\n--- LICH SU DON HANG CỦA " << username << " ---" << endl;
        if (orderHistory.empty()) {
            cout << "Ban chua co don hang nao." << endl;
            return;
        }
        for (const auto& o : orderHistory) {
            o.displayOrder();
            cout << "----------------------------------------" << endl;
        }
    }
};

class Admin : public User {
public:
    Admin(string u, string p, string e) : User(u, p, e) {}
    void displayRole() const override {
        cout << "Quan tri vien: " << username << " (" << email << ")" << endl;
    }
    void manageInventory(Inventory& inv) {
        inv.addProduct(make_shared<ElectronicsProd>("E01", "Laptop ASUS ROG", 1200.0, 24), 5);
        inv.addProduct(make_shared<ElectronicsProd>("E02", "iPhone 15 Pro", 1000.0, 12), 8);
        inv.addProduct(make_shared<ClothingProd>("C01", "Ao khoac Bomber", 50.0, "L"), 15);
        inv.addProduct(make_shared<ClothingProd>("C02", "Quan Jean Levi's", 80.0, "32"), 20);
        cout << "\n[Admin] Da tu dong khoi tao va nhap kho san pham!" << endl;
    }
};

int main() {
    Inventory storeInventory;
    
    Admin admin("admin", "admin123", "admin@shop.com");
    vector<shared_ptr<Customer>> customerList;
    customerList.push_back(make_shared<Customer>("hoang123", "123456", "hoang@gmail.com"));

    admin.manageInventory(storeInventory);

    shared_ptr<Customer> currentCustomer = nullptr;
    bool isAdminLoggedIn = false;
    int orderCounter = 1000;

    while (true) {
        if (!currentCustomer && !isAdminLoggedIn) {
            cout << "\n========================================" << endl;
            cout << "   HE THONG MUA SAM TRUONG TRUC TUYEN   " << endl;
            cout << "========================================" << endl;
            cout << "1. Dang nhap Khach hang" << endl;
            cout << "2. Dang ky tai khoan moi" << endl;
            cout << "3. Dang nhap Admin (Quan ly kho)" << endl;
            cout << "4. Thoat chuong trình" << endl;
            cout << "Lua chon cua ban: ";
            int choice;
            cin >> choice;

            if (choice == 1) {
                string u, p;
                cout << "Username: "; cin >> u;
                cout << "Password: "; cin >> p;
                
                for (auto& cust : customerList) {
                    if (cust->getUsername() == u && cust->checkPassword(p)) {
                        currentCustomer = cust;
                        break;
                    }
                }
                if (currentCustomer) {
                    cout << "\nDang nhap thanh cong! Chao mung " << currentCustomer->getUsername() << endl;
                } else {
                    cout << "\nSai tai khoan hoac mat khau!" << endl;
                }
            } 
            else if (choice == 2) {
                string u, p, e;
                cout << "Nhap username moi: "; cin >> u;
                cout << "Nhap password: "; cin >> p;
                cout << "Nhap email: "; cin >> e;
                customerList.push_back(make_shared<Customer>(u, p, e));
                cout << "\nDang ky thanh con! Ban co the dang nhap ngay." << endl;
            } 
            else if (choice == 3) {
                string u, p;
                cout << "Admin Username: "; cin >> u;
                cout << "Admin Password: "; cin >> p;
                if (admin.getUsername() == u && admin.checkPassword(p)) {
                    isAdminLoggedIn = true;
                    cout << "\nDang nhap quyen Admin thanh cong!" << endl;
                } else {
                    cout << "\nSai thong tin Admin!" << endl;
                }
            } 
            else if (choice == 4) {
                cout << "\nTam biet ban!" << endl;
                break;
            }
        } 
        else if (isAdminLoggedIn) {
            cout << "\n--- MENU QUAN TRI VIEN (ADMIN) ---" << endl;
            cout << "1. Xem danh sach hang trong kho" << endl;
            cout << "2. Nhap them hang vao kho" << endl;
            cout << "3. Dang xuat" << endl;
            cout << "Lua chon: ";
            int admChoice;
            cin >> admChoice;

            if (admChoice == 1) {
                storeInventory.viewInventory();
            } 
            else if (admChoice == 2) {
                string id, name;
                int type, qty;
                double price;
                cout << "Chon loai san pham (1- Dien tu, 2- Thoi trang): "; cin >> type;
                cout << "Nhap ma ID: "; cin >> id;
                cout << "Nhap ten SP (khong dau, dung dau_ gach ngang): "; cin >> name;
                cout << "Nhap gia base: "; cin >> price;
                cout << "Nhap so luong nhap kho: "; cin >> qty;
                
                if (type == 1) {
                    int warranty;
                    cout << "Nhap so thang bao hanh: "; cin >> warranty;
                    storeInventory.addProduct(make_shared<ElectronicsProd>(id, name, price, warranty), qty);
                } else {
                    string size;
                    cout << "Nhap size (S/M/L/XL): "; cin >> size;
                    storeInventory.addProduct(make_shared<ClothingProd>(id, name, price, size), qty);
                }
                cout << "\nNhap hang thanh cong!" << endl;
            } 
            else {
                isAdminLoggedIn = false;
                cout << "\nDa dang xuat khoi tai khoan Admin." << endl;
            }
        } 
        else if (currentCustomer) {
            // ĐÃ SỬA: Đưa dòng khởi tạo giỏ hàng ra ngoài vòng lặp Menu hành động của khách hàng
            Order currentCart("ORD" + to_string(++orderCounter));
            
            while (currentCustomer) {
                cout << "\n--- MENU KHACH HANG ---" << endl;
                cout << "1. Xem danh sach san pham shop co" << endl;
                cout << "2. Them san pham vao Gio hang" << endl;
                cout << "3. Xem Gio hang hien tai" << endl;
                cout << "4. Thanh toan don hang" << endl;
                cout << "5. Xem lich su don hang da mua" << endl;
                cout << "6. Dang xuat" << endl;
                cout << "Lua chon: ";
                int custChoice;
                cin >> custChoice;

                if (custChoice == 1) {
                    storeInventory.viewInventory();
                } 
                else if (custChoice == 2) {
                    string id;
                    int qty;
                    cout << "Nhap Ma ID san pham muon mua: "; cin >> id;
                    cout << "Nhap so luong: "; cin >> qty;

                    shared_ptr<Product> prod = storeInventory.findProduct(id);
                    if (prod) {
                        if (storeInventory.deductStock(id, qty)) {
                            currentCart.addItem(prod, qty);
                            cout << "\nDa them " << prod->getName() << " vao gio hang!" << endl;
                        } else {
                            cout << "\nSo luong trong kho khong du!" << endl;
                        }
                    } else {
                        cout << "\nSan pham khong ton tai hoac da het hang!" << endl;
                    }
                } 
                else if (custChoice == 3) {
                    cout << "\n--- GIO HANG CUA BAN ---" << endl;
                    currentCart.displayOrder();
                } 
                else if (custChoice == 4) {
                    if (currentCart.isEmpty()) {
                        cout << "\nGio hang dang trong. Vui long them san pham truoc!" << endl;
                        continue;
                    }
                    cout << "\nChon phuong thuc thanh toan:" << endl;
                    cout << "1. Credit Card (The tin dung)" << endl;
                    cout << "2. E-Wallet (Vi dien tu)" << endl;
                    cout << "Lua chon phuong thuc: ";
                    int payChoice;
                    cin >> payChoice;

                    unique_ptr<PaymentStrategy> paymentMethod = nullptr;
                    if (payChoice == 1) {
                        string cardNum;
                        cout << "Nhap so the: "; cin >> cardNum;
                        paymentMethod = make_unique<CreditCard>(cardNum);
                    } else {
                        string phone;
                        cout << "Nhap so dien thoai vi: "; cin >> phone;
                        paymentMethod = make_unique<EWallet>(phone);
                    }

                    currentCart.processPayment(move(paymentMethod));
                    currentCustomer->addOrder(currentCart);
                    
                    currentCart = Order("ORD" + to_string(++orderCounter)); 
                } 
                else if (custChoice == 5) {
                    currentCustomer->viewOrderHistory();
                } 
                else {
                    currentCustomer = nullptr;
                    cout << "\nDa dang xuat khach hang." << endl;
                }
            }
        }
    }
    return 0;
}