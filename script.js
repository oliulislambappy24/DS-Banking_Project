// ডেটা স্টোরেজ
let accounts = JSON.parse(localStorage.getItem('brm_accounts')) || [];
let transactions = JSON.parse(localStorage.getItem('brm_transactions')) || [];
let currentAcc = null;
let currentAction = "";

// শুরুতে চয়েস হ্যান্ডলিং
function selectOption(type) {
    document.getElementById('choice-section').classList.add('hidden');
    document.getElementById('auth-section').classList.remove('hidden');
    
    if (type === 'register') {
        setRegisterUI();
    } else {
        setLoginUI();
    }
}

function goBack() {
    document.getElementById('auth-section').classList.add('hidden');
    document.getElementById('choice-section').classList.remove('hidden');
}

// UI State Management
function setLoginUI() {
    document.getElementById('form-title').innerText = "BRM Bank Login";
    document.getElementById('main-btn').innerText = "Login Now";
    document.getElementById('extra-fields').classList.add('hidden');
    document.getElementById('toggle-text').innerHTML = 'New to BRM? <a href="javascript:void(0)" onclick="toggleAuth()">Create Account</a>';
}

function setRegisterUI() {
    document.getElementById('form-title').innerText = "Create Account";
    document.getElementById('main-btn').innerText = "Register Now";
    document.getElementById('extra-fields').classList.remove('hidden');
    document.getElementById('toggle-text').innerHTML = 'Already have an account? <a href="javascript:void(0)" onclick="toggleAuth()">Login</a>';
}

function toggleAuth() {
    const isLogin = document.getElementById('form-title').innerText === "BRM Bank Login";
    if (isLogin) {
        setRegisterUI();
    } else {
        setLoginUI();
    }
}

// Auth Functions
function handleAuth() {
    const accNo = parseInt(document.getElementById('accNo').value);
    const name = document.getElementById('accName').value;

    if (!accNo || !name) return alert("Please fill the required fields!");

    if (document.getElementById('main-btn').innerText === "Login Now") {
        const user = accounts.find(a => a.accNo === accNo && a.name === name);
        if (user) {
            currentAcc = user;
            loadDashboard();
        } else {
            alert("Login Failed! Account not found or name mismatch.");
        }
    } else {
        if (accounts.find(a => a.accNo === accNo)) return alert("Account number already exists!");
        
        const phone = document.getElementById('accPhone').value;
        const address = document.getElementById('accAddress').value;
        
        if (!phone || !address) return alert("Please fill all fields for registration!");

        accounts.push({
            accNo, name, balance: 0,
            phone: phone,
            address: address
        });
        saveData();
        alert("Account Created successfully! Now login.");
        setLoginUI();
    }
}

function loadDashboard() {
    document.getElementById('auth-section').classList.add('hidden');
    document.getElementById('choice-section').classList.add('hidden');
    document.getElementById('dashboard').classList.remove('hidden');
    document.getElementById('user-display').innerText = currentAcc.name;
    document.getElementById('balance-display').innerText = currentAcc.balance.toFixed(2);
}

// Banking Actions
function showModal(action) {
    currentAction = action;
    document.getElementById('modal-title').innerText = action.toUpperCase();
    document.getElementById('modal').classList.remove('hidden'); 
    
    const transOpt = document.getElementById('transfer-options');
    transOpt.classList.toggle('hidden', action !== 'transfer');
    if(action === 'transfer') toggleTransferFields();
}

function closeModal() {
    document.getElementById('modal').classList.add('hidden'); 
    document.getElementById('amount-input').value = "";
}

function toggleTransferFields() {
    const type = document.getElementById('transfer-type').value;
    document.getElementById('field-this-bank').classList.toggle('hidden', type !== 'this-bank');
    document.getElementById('field-other-bank').classList.toggle('hidden', type !== 'other-bank');
    document.getElementById('field-mobile').classList.toggle('hidden', type !== 'mobile');
}

function processAction() {
    const amount = parseFloat(document.getElementById('amount-input').value);
    if (isNaN(amount) || amount <= 0) return alert("Please enter a valid amount");

    if (currentAction === 'deposit') {
        currentAcc.balance += amount;
        addLog("Deposit", amount);
    } 
    else if (currentAction === 'withdraw') {
        if (currentAcc.balance < amount) return alert("Insufficient balance!");
        currentAcc.balance -= amount;
        addLog("Withdraw", amount);
    } 
    else if (currentAction === 'transfer') {
        if (currentAcc.balance < amount) return alert("Insufficient balance!");
        const type = document.getElementById('transfer-type').value;
        
        if (type === 'this-bank') {
            const revNo = parseInt(document.getElementById('receiver-acc').value);
            const rev = accounts.find(a => a.accNo === revNo);
            if (!rev) return alert("Receiver account not found!");
            currentAcc.balance -= amount; rev.balance += amount;
            addLog(`Transfer to ${revNo}`, amount);
        } else {
            currentAcc.balance -= amount;
            addLog(`External Transfer`, amount);
        }
    }

    saveData();
    updateBalanceUI();
    closeModal(); 
}

function updateBalanceUI() {
    document.getElementById('balance-display').innerText = currentAcc.balance.toFixed(2);
}

function addLog(type, amount) {
    transactions.push({ accNo: currentAcc.accNo, type, amount, time: Date.now() });
    localStorage.setItem('brm_transactions', JSON.stringify(transactions));
}

function displayTransactions() {
    const list = document.getElementById('history-list');
    const historySection = document.getElementById('transaction-history');
    historySection.classList.toggle('hidden');
    
    if (historySection.classList.contains('hidden')) return;

    list.innerHTML = "";
    const recent = transactions.filter(t => t.accNo === currentAcc.accNo);
    
    if(recent.length === 0) {
        list.innerHTML = "<li>No transactions found.</li>";
    } else {
        recent.reverse().slice(0, 10).forEach(t => {
            const time = new Date(t.time).toLocaleTimeString();
            list.innerHTML += `<li><i class="fas fa-history" style="color:var(--primary); font-size:10px; margin-right:8px"></i> [${time}] ${t.type}: ${t.amount} TK</li>`;
        });
    }
}

function saveData() { 
    localStorage.setItem('brm_accounts', JSON.stringify(accounts)); 
}

function logout() { 
    location.reload(); 
}