const $=id=>document.getElementById(id);
let account=null,toastTimer;
const money=n=>new Intl.NumberFormat('en-IN',{style:'currency',currency:'INR'}).format(Number(n||0));
function toast(msg){$('toast').textContent=msg;$('toast').classList.add('show');clearTimeout(toastTimer);toastTimer=setTimeout(()=>$('toast').classList.remove('show'),2800)}
function show(view){
 $('loginView').hidden=!!account;
 $('appView').hidden=!account||view!=='dashboard';
 $('transactionView').hidden=!account||view!=='transactions';
 $('transferView').hidden=!account||view!=='transfer';
 document.querySelectorAll('.nav').forEach(b=>b.classList.toggle('active',b.dataset.view===view));
 $('heading').textContent=view==='transactions'?'Transaction history':view==='transfer'?'Transfer money':'Good afternoon';
}
async function api(path,opts={}){const r=await fetch(path,{...opts,headers:{'Content-Type':'application/json',...(opts.headers||{})}});const txt=await r.text();if(!r.ok)throw new Error(txt||'Request failed');return txt?JSON.parse(txt):{}}
async function loadAccount(){
 const data=await api('/api/account/'+account.accountNo);account={...account,...data};
 $('userName').textContent=account.holder;$('balance').textContent=money(account.balance);
 $('accountType').textContent=account.type;$('accountMeta').textContent='Account •••• '+account.accountNo;
 $('last4').textContent=String(account.accountNo).slice(-4);
 await loadTransactions();
}
async function loadTransactions(){
 const data=await api('/api/transactions/'+account.accountNo);
 const rows=data.transactions||[];
 const render=rows.length?rows.map(t=>`<tr><td>${escapeHtml(t.details||t.type)}</td><td>${escapeHtml(t.date||'—')}</td><td class="${/IN|DEPOSIT/.test(t.type)?'amount-in':'amount-out'}">${/IN|DEPOSIT/.test(t.type)?'+':'−'}${money(t.amount)}</td><td><span class="status">Completed</span></td></tr>`).join(''):'<tr><td colspan="4" class="empty">No transactions yet.</td></tr>';
 $('recentRows').innerHTML=render;$('allRows').innerHTML=render;
}
function escapeHtml(s){return String(s).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]))}
$('loginForm').addEventListener('submit',async e=>{e.preventDefault();$('loginError').textContent='';
 try{const data=await api('/api/login',{method:'POST',body:JSON.stringify({accountNo:Number($('accountNo').value),pin:$('pin').value})});
 account={accountNo:Number($('accountNo').value),...data};await loadAccount();show('dashboard');toast('Welcome to NovaBank');}
 catch(err){$('loginError').textContent=err.message}
});
document.querySelectorAll('[data-view]').forEach(b=>b.addEventListener('click',()=>{if(!account)return;show(b.dataset.view)}));
document.querySelectorAll('[data-action]').forEach(b=>b.addEventListener('click',()=>openMoneyModal(b.dataset.action)));
$('logout').addEventListener('click',()=>{account=null;show('dashboard');$('pin').value='';toast('You have been logged out')});
$('transferForm').addEventListener('submit',async e=>{e.preventDefault();$('transferMsg').textContent='';
 try{await api('/api/transfer',{method:'POST',body:JSON.stringify({from:account.accountNo,to:Number($('recipient').value),amount:Number($('transferAmount').value)})});
 $('transferMsg').textContent='Transfer successful.';toast('Transfer completed');await loadAccount();$('transferForm').reset();}
 catch(err){$('transferMsg').textContent=err.message}
});
function openMoneyModal(type){
 const title=type==='deposit'?'Deposit money':'Withdraw money';
 $('modalContent').innerHTML=`<p class="eyebrow">ACCOUNT ACTION</p><h2>${title}</h2><p class="sub">Account ${account.accountNo}</p><form id="moneyForm"><label>Amount (₹)</label><input id="moneyAmount" type="number" min="1" step="0.01" placeholder="0.00" required><button class="primary full">${title} →</button><p id="moneyMsg" class="message"></p></form>`;
 $('modal').hidden=false;
 $('moneyForm').addEventListener('submit',async e=>{e.preventDefault();try{await api('/api/'+type,{method:'POST',body:JSON.stringify({accountNo:account.accountNo,amount:Number($('moneyAmount').value)})});$('modal').hidden=true;toast(title+' successful');await loadAccount();}catch(err){$('moneyMsg').textContent=err.message}});
}
$('closeModal').addEventListener('click',()=>$('modal').hidden=true);$('modal').addEventListener('click',e=>{if(e.target===$('modal'))$('modal').hidden=true});
$('today').textContent=new Intl.DateTimeFormat('en-IN',{dateStyle:'medium'}).format(new Date());
show('dashboard');
