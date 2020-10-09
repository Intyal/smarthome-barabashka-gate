//document.addEventListener('DOMContentLoaded', loadContent('page_main.html'));
$(document).ready(function(){
	components_init();
});

function components_init() {
    M.AutoInit();
    $('select').formSelect();
	$('.sidenav').sidenav();
	$('input[data-length]').characterCounter();
	
}

function components_update() {
	M.updateTextFields();
	$('select').formSelect();
	$('.collapsible').collapsible();
}

// Загрузка страницы в блок
function getPage(src, call_elem) {
	console.log(call_elem.getElementsByClassName('caption-menu')[0].textContent);
	let page = loadData('page_' + src + '.html');
	page
		.then(function(fulfilled) {
			// Вставить загруженный контент в элемент
			let main_content = document.getElementById('main_content');
			main_content.innerHTML = fulfilled;
			// Присвоить текст заголовока из пункта меню
			let nav_title = document.getElementsByClassName('nav-title');
			let caption_menu = call_elem.getElementsByClassName('caption-menu')[0];
			for (let title of nav_title) {
				title.textContent = caption_menu.textContent;
			}
			// Переключить подсветку на выбранное меню
			let toggle_menu = document.getElementsByClassName('toggle-menu');
			for (let item of toggle_menu) {
				item.classList.remove('active');
			}
			call_elem.classList.add('active');
			// Инициализация компонентов CSS фреймворка
			components_init();
			// Запуск скрипта из загруженного шаблона
			startScript();
		})
		.catch(function(error) {
			console.log(error.message)
		});
}

// Получение данных с сервевра
function loadData(src) {
    return new Promise(function(resolve, reject) {
        let request = new XMLHttpRequest();
        request.open('GET', src, true);
        request.send();

        request.onreadystatechange = function() {
            if (this.readyState != 4) return;
            // по окончании запроса доступны:
            // status, statusText
            // responseText, responseXML (при content-type: text/xml)
            if (this.status != 200) {
                // обработать ошибку
                reject(new Error(`Ошибка ${(this.status ? this.statusText : 'запрос не удался')}`));
            }
            // получить результат из this.responseText или this.responseXML
            resolve(this.responseText);
        }
    });
}

// Добавление на страницу подгруженных скриптов и запуск скрипта page_script
function startScript() {
    let script = document.createElement('script');
    script.innerHTML = document.getElementById('template_script').innerHTML;
    document.getElementById('container_script').append(script);
    window['page_script']();
}

Handlebars.registerHelper('json_string', function (data) {
    return JSON.stringify(data);
})